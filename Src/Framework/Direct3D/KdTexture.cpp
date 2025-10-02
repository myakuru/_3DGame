#include "Framework/KdFramework.h"
#define NANOSVG_IMPLEMENTATION
#define NANOSVGRAST_IMPLEMENTATION
#include <nanosvg.h>
#include <nanosvgrast.h>

#include "KdTexture.h"

static bool RasterizeSvgToRgba(const char* path, int targetW, int targetH,
	std::vector<uint8_t>& outRGBA, int& outW, int& outH)
{
	const float dpi = 96.0f;
	NSVGimage* svg = nsvgParseFromFile(path, "px", dpi);
	if (!svg) return false;

	// フォールバックサイズ（width/height が 0 の SVG 対応）
	const int kFallbackSize = 512;

	// 出力サイズ決定
	const bool hasW = (svg->width > 0.0f);
	const bool hasH = (svg->height > 0.0f);

	outW = (targetW > 0) ? targetW : (hasW ? static_cast<int>(std::ceil(svg->width)) : kFallbackSize);
	outH = (targetH > 0) ? targetH : (hasH ? static_cast<int>(std::ceil(svg->height)) : kFallbackSize);
	if (outW <= 0 || outH <= 0) { nsvgDelete(svg); return false; }

	// スケール決定（0 除算回避）
	const float sx = hasW ? (static_cast<float>(outW) / svg->width) : 1.0f;
	const float sy = hasH ? (static_cast<float>(outH) / svg->height) : 1.0f;
	const float scale = std::min(sx, sy);

	NSVGrasterizer* rast = nsvgCreateRasterizer();
	if (!rast) { nsvgDelete(svg); return false; }

	outRGBA.resize(static_cast<size_t>(outW) * outH * 4);
	nsvgRasterize(rast, svg, 0.0f, 0.0f, scale, outRGBA.data(), outW, outH, outW * 4);

	nsvgDeleteRasterizer(rast);
	nsvgDelete(svg);
	return true;
}


// 2D画像(resource)リソースから、最適なビューを作成する
// ・resource		… 2D画像リソース
// ・ppSRV			… 作成されたShaderResourceViewを受け取るための変数のアドレス
// ・ppRTV			… 作成されたRenderTargetViewを受け取るための変数のアドレス
// ・ppDSV			… 作成されたDepthStencilViewを受け取るための変数のアドレス
static bool KdCreateViewsFromTexture2D(ID3D11Texture2D* resource, ID3D11ShaderResourceView** ppSRV, ID3D11RenderTargetView** ppRTV, ID3D11DepthStencilView** ppDSV)
{
	// リソースが無い
	if (resource == nullptr)return false;

	// テクスチャ本体の情報取得
	D3D11_TEXTURE2D_DESC desc;
	resource->GetDesc(&desc);

	//===========================================================
	//
	// RenderTargetViewを作成する
	//
	//===========================================================

	// レンダーターゲットフラグがついてる時のみ
	if (ppRTV && desc.BindFlags & D3D11_BIND_RENDER_TARGET)
	{
		// 作成するビューの設定
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = desc.Format;	// Format
		// 単品のテクスチャ(通常テクスチャ)の場合
		if (desc.ArraySize == 1) {
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;			// 単品テクスチャ
		}
		// テクスチャ配列の場合
		else {
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;		// テクスチャ配列
			rtvDesc.Texture2DArray.ArraySize = desc.ArraySize;			// 要素数
			rtvDesc.Texture2DArray.FirstArraySlice = 0;
			rtvDesc.Texture2DArray.MipSlice = 0;
		}

		// レンダーターゲットビュー作成
		HRESULT hr = KdDirect3D::Instance().WorkDev()->CreateRenderTargetView(resource, &rtvDesc, ppRTV);
		if (FAILED(hr))
		{
			assert(0 && "RenderTargetViewの作成に失敗");
			return false;
		}
	}

	//===========================================================
	//
	// ShaderResourceViewの情報を作成する
	//
	//===========================================================
	// シェーダリソースビューフラグがついてる時のみ
	if (ppSRV && desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		// 作成するビューの設定
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

		// テクスチャがZバッファの場合は、最適なフォーマットにする
		if (desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
		{
			switch (desc.Format) {
				// 16ビット
			case DXGI_FORMAT_R16_TYPELESS:
				srvDesc.Format = DXGI_FORMAT_R16_UNORM;
				break;
				// 32ビット
			case DXGI_FORMAT_R32_TYPELESS:
				srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
				break;
				// 24ビット(Zバッファ) + 8ビット(ステンシルバッファ) 
			case DXGI_FORMAT_R24G8_TYPELESS:
				srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
				break;
			default:
				assert(0 && "[ShaderResource] 対応していないフォーマットです");
				break;
			}
		}
		// Zバッファでない場合は、そのまま同じフォーマットを使用
		else
		{
			srvDesc.Format = desc.Format;
		}

		// 単品のテクスチャ(通常テクスチャ)の場合
		if (desc.ArraySize == 1)
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = desc.MipLevels;
			if (srvDesc.Texture2D.MipLevels <= 0)srvDesc.Texture2D.MipLevels = (UINT)-1;
		}
		// テクスチャ配列の場合
		else {
			// さらにキューブマップの場合
			if (desc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE) {
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			}
			// 通常テクスチャ配列
			else {
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			}
			srvDesc.Texture2DArray.MostDetailedMip = 0;
			srvDesc.Texture2DArray.MipLevels = desc.MipLevels;
			srvDesc.Texture2DArray.ArraySize = desc.ArraySize;	// 要素数
			srvDesc.Texture2DArray.FirstArraySlice = 0;
		}

		// シェーダリソースビュー作成
		HRESULT hr = KdDirect3D::Instance().WorkDev()->CreateShaderResourceView(resource, &srvDesc, ppSRV);
		if (FAILED(hr))
		{
			assert(0 && "ShaderResourceViewの作成に失敗");
			return false;
		}
	}

	//===========================================================
	//
	// DepthStencilViewを作成する
	//
	//===========================================================
	// Zバッファフラグがついてる時のみ
	if (ppDSV && desc.BindFlags & D3D11_BIND_DEPTH_STENCIL) {
		// 作成するビューの設定
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};

		// テクスチャー作成時に指定したフォーマットと互換性があり、深度ステンシルビューとして指定できるフォーマットを指定する
		switch (desc.Format) {
			// 16ビット
		case DXGI_FORMAT_R16_TYPELESS:
			dsvDesc.Format = DXGI_FORMAT_D16_UNORM;
			break;
			// 32ビット
		case DXGI_FORMAT_R32_TYPELESS:
			dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
			break;
			// 24ビット(Zバッファ) + 8ビット(ステンシルバッファ) 
		case DXGI_FORMAT_R24G8_TYPELESS:
			dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			break;
		default:
			assert(0 && "[DepthStencil] 対応していないフォーマットです");
			break;
		}

		// 単品のテクスチャ(通常テクスチャ)の場合
		if (desc.ArraySize == 1) {
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Texture2D.MipSlice = 0;
		}
		// テクスチャ配列の場合
		else {
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			dsvDesc.Texture2DArray.ArraySize = desc.ArraySize;
			dsvDesc.Texture2DArray.MipSlice = 0;
			dsvDesc.Texture2DArray.FirstArraySlice = 0;
		}

		//-------------------------------
		// デプスステンシルビュー作成
		//-------------------------------
		HRESULT hr = KdDirect3D::Instance().WorkDev()->CreateDepthStencilView(resource, &dsvDesc, ppDSV);
		if (FAILED(hr)) {
			assert(0 && "DepthStencilViewの作成に失敗");
			return false;
		}
	}

	return true;
}


// リソースを取得
const ID3D11Texture2D* KdTexture::GetResource() const
{
	return WorkResource();
}

ID3D11Texture2D* KdTexture::WorkResource() const
{
	if (m_srv == nullptr)return nullptr;

	ID3D11Resource* res;
	m_srv->GetResource(&res);

	ID3D11Texture2D* tex2D;
	if (FAILED(res->QueryInterface<ID3D11Texture2D>(&tex2D)))
	{
		res->Release();
		return nullptr;
	}
	res->Release();
	tex2D->Release();

	return tex2D;
}

bool KdTexture::Load(std::string_view filename, bool renderTarget, bool depthStencil, bool generateMipmap)
{
	Release();
	if (filename.empty())return false;

	// ヌル終端文字列を確保（C API 対応）
	const std::string pathStr(filename);

	// 拡張子（小文字）を取得
	auto getLowerExt = [](std::string_view path) -> std::string {
		const size_t dot = path.find_last_of('.');
		if (dot == std::string_view::npos) return {};
		std::string ext(path.substr(dot + 1));
		for (auto& c : ext) c = static_cast<char>(::tolower(static_cast<unsigned char>(c)));
		return ext;
		};
	const std::string ext = getLowerExt(filename);

	// SVG なら NanoSVG でラスタライズしてテクスチャ作成
	if (ext == "svg")
	{
		// ここで表示予定ピクセルに合わせて targetW/targetH を渡す
		return LoadSvg(filename, m_desc.Width, m_desc.Height, renderTarget, generateMipmap);
	}

	// 以降は従来の DirectXTex/WIC 経路
	// ファイル名をWideCharへ変換（ヌル終端の c_str を渡す）
	std::wstring wFilename = sjis_to_wide(pathStr.c_str());

	// Bind Flags
	UINT bindFlags = 0;
	bindFlags |= D3D11_BIND_SHADER_RESOURCE;
	if (renderTarget)bindFlags |= D3D11_BIND_RENDER_TARGET;
	if (depthStencil)bindFlags |= D3D11_BIND_DEPTH_STENCIL;

	DirectX::TexMetadata meta;
	DirectX::ScratchImage image;

	bool bLoaded = false;

	// WIC画像読み込み
	if (SUCCEEDED(DirectX::LoadFromWICFile(wFilename.c_str(), DirectX::WIC_FLAGS_ALL_FRAMES, &meta, image)))
	{
		bLoaded = true;
	}

	// DDS画像読み込み
	if (!bLoaded) {
		if (SUCCEEDED(DirectX::LoadFromDDSFile(wFilename.c_str(), DirectX::DDS_FLAGS_NONE, &meta, image)))
		{
			bLoaded = true;
		}
	}

	// TGA画像読み込み
	if (!bLoaded) {
		if (SUCCEEDED(DirectX::LoadFromTGAFile(wFilename.c_str(), &meta, image)))
		{
			bLoaded = true;
		}
	}

	// HDR画像読み込み
	if (!bLoaded) {
		if (SUCCEEDED(DirectX::LoadFromHDRFile(wFilename.c_str(), &meta, image)))
		{
			bLoaded = true;
		}
	}

	// 読み込み失敗
	if (!bLoaded)
	{
		return false;
	}

	// ミップマップ生成（SVG以外）
	if (meta.mipLevels == 1 && generateMipmap)
	{
		DirectX::ScratchImage mipChain;
		if (SUCCEEDED(DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, mipChain)))
		{
			image.Release();
			image = std::move(mipChain);
		}
	}

	// テクスチャ作成
	ID3D11Texture2D* tex2D = nullptr;
	if (FAILED(DirectX::CreateTextureEx(
		KdDirect3D::Instance().WorkDev(),
		image.GetImages(),
		image.GetImageCount(),
		image.GetMetadata(),
		D3D11_USAGE_DEFAULT,
		bindFlags,
		0,
		0,
		DirectX::CREATETEX_FLAGS::CREATETEX_DEFAULT,
		(ID3D11Resource**)&tex2D)
	)) {

		return false;
	}

	// 各ビュー作成
	if (KdCreateViewsFromTexture2D(tex2D, &m_srv, &m_rtv, &m_dsv) == false)
	{
		tex2D->Release();
		Release();
		return false;
	}

	// 画像情報取得
	tex2D->GetDesc(&m_desc);
	tex2D->Release();

	m_filepath = pathStr;

	return true;
}

bool KdTexture::Create(ID3D11Texture2D* pTexture2D)
{
	Release();
	if (pTexture2D == nullptr)return false;

	//---------------------------------------------
	// 画像リソースから、各ビューを作成する
	//---------------------------------------------
	if (KdCreateViewsFromTexture2D(pTexture2D, &m_srv, &m_rtv, &m_dsv) == false) {
		Release();
		return false;
	}

	// ファイル名記憶
	m_filepath = "";

	// 画像情報取得
	pTexture2D->GetDesc(&m_desc);

	return true;

}

bool KdTexture::Create(const D3D11_TEXTURE2D_DESC & desc, const D3D11_SUBRESOURCE_DATA * fillData)
{
	Release();

	//--------------------------------------------
	// 2Dテクスチャの生成
	//--------------------------------------------
	ID3D11Texture2D* resource;
	HRESULT hr = KdDirect3D::Instance().WorkDev()->CreateTexture2D(&desc, fillData, &resource);
	if (FAILED(hr)) {
		Release();
		return false;
	}

	//--------------------------------------------
	// m_pTexture2Dから、各ビューを作成する
	//--------------------------------------------
	if (KdCreateViewsFromTexture2D(resource, &m_srv, &m_rtv, &m_dsv) == false)
	{
		Release();
		return false;
	}

	// 画像情報取得
	resource->GetDesc(&m_desc);
	resource->Release();

	return true;
}

void SetTextureInfo(D3D11_TEXTURE2D_DESC& rDesc, int w, int h, DXGI_FORMAT format, UINT arrayCnt, UINT miscFlags)
{
	rDesc.Width = (UINT)w;
	rDesc.Height = (UINT)h;
	rDesc.Format = format;
	rDesc.ArraySize = arrayCnt;
	rDesc.MiscFlags = miscFlags;
}

bool KdTexture::Create(int w, int h, DXGI_FORMAT format, UINT arrayCnt, const D3D11_SUBRESOURCE_DATA * fillData)
{
	Release();


	// 作成する2Dテクスチャ設定
	D3D11_TEXTURE2D_DESC desc = {};

	SetTextureInfo(desc, w, h, format, arrayCnt, 0);

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	// 作成
	if (Create(desc, fillData) == false)return false;

	return true;
}

bool KdTexture::CreateRenderTarget(int w, int h, DXGI_FORMAT format, UINT arrayCnt, const D3D11_SUBRESOURCE_DATA* fillData, UINT miscFlags)
{
	Release();

	// 作成する2Dテクスチャ設定
	D3D11_TEXTURE2D_DESC desc = {};

	SetTextureInfo(desc, w, h, format, arrayCnt, 0);
	
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.MiscFlags = miscFlags;

	// 作成
	if (Create(desc, fillData) == false)return false;

	return true;
}

bool KdTexture::CreateDepthStencil(int w, int h, DXGI_FORMAT format, UINT arrayCnt, const D3D11_SUBRESOURCE_DATA* fillData, UINT miscFlags)
{
	Release();

	// 作成する2Dテクスチャ設定
	D3D11_TEXTURE2D_DESC desc = {};

	SetTextureInfo(desc, w, h, format, arrayCnt, 0);

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	desc.CPUAccessFlags = 0;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.MiscFlags = miscFlags;

	// 作成
	if (Create(desc, fillData) == false)return false;

	return true;
}

void KdTexture::SetSRView(ID3D11ShaderResourceView* srv)
{
	if (srv == nullptr)return;

	Release();
	m_srv = srv;	// セット
	m_srv->AddRef();// 参照カウンタを増やしておく

	// 画像情報取得
	ID3D11Resource* res = nullptr;
	m_srv->GetResource(&res);
	if (res) {
		ID3D11Texture2D* tex2D = nullptr;
		if (SUCCEEDED(res->QueryInterface<ID3D11Texture2D>(&tex2D))) {
			tex2D->GetDesc(&m_desc);
			tex2D->Release();
		}
		res->Release();
	}
}

void KdTexture::Release()
{
	KdSafeRelease(m_srv);
	KdSafeRelease(m_rtv);
	KdSafeRelease(m_dsv);

	m_filepath = "";
}

bool KdTexture::LoadSvg(std::string_view filename, int targetW, int targetH, bool renderTarget, bool generateMipmap)
{
	Release();
	if (filename.empty()) return false;

	const std::string pathStr(filename);

	int w = 0, h = 0;
	std::vector<uint8_t> rgba;
	if (!RasterizeSvgToRgba(pathStr.c_str(), targetW, targetH, rgba, w, h)) {
		return false;
	}

	ID3D11Texture2D* tex2D = nullptr;

	if (generateMipmap) {
		// CPUでミップチェーンを生成（縮小品質を改善）
		DirectX::Image base{};
		base.width = static_cast<size_t>(w);
		base.height = static_cast<size_t>(h);
		base.format = DXGI_FORMAT_R8G8B8A8_UNORM; // 必要なら _SRGB へ
		base.rowPitch = static_cast<size_t>(w) * 4;
		base.slicePitch = base.rowPitch * static_cast<size_t>(h);
		base.pixels = rgba.data();

		DirectX::ScratchImage mipChain;
		if (FAILED(DirectX::GenerateMipMaps(base, DirectX::TEX_FILTER_DEFAULT, 0, mipChain))) {
			return false;
		}

		if (FAILED(DirectX::CreateTextureEx(
			KdDirect3D::Instance().WorkDev(),
			mipChain.GetImages(),
			mipChain.GetImageCount(),
			mipChain.GetMetadata(),
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_SHADER_RESOURCE | (renderTarget ? D3D11_BIND_RENDER_TARGET : 0),
			0,
			0,
			DirectX::CREATETEX_FLAGS::CREATETEX_DEFAULT,
			reinterpret_cast<ID3D11Resource**>(&tex2D)))) {
			return false;
		}
	}
	else {
		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = static_cast<UINT>(w);
		desc.Height = static_cast<UINT>(h);
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | (renderTarget ? D3D11_BIND_RENDER_TARGET : 0);

		D3D11_SUBRESOURCE_DATA srd{};
		srd.pSysMem = rgba.data();
		srd.SysMemPitch = static_cast<UINT>(w * 4);

		if (!Create(desc, &srd)) { return false; }

		m_filepath = pathStr;
		return true;
	}

	// 各ビュー作成
	if (!KdCreateViewsFromTexture2D(tex2D, &m_srv, &m_rtv, &m_dsv)) {
		tex2D->Release();
		Release();
		return false;
	}

	tex2D->GetDesc(&m_desc);
	tex2D->Release();

	m_filepath = pathStr;
	return true;
}


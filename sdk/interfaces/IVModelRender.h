#pragma once

struct model_t {
	char        name[255];
};
typedef unsigned short ModelInstanceHandle_t;

enum modtype_t {
	mod_bad = 0,
	mod_brush,
	mod_sprite,
	mod_studio
};

struct ModelRenderInfo_t {
	Vector origin;
	QAngle angles;
	char pad[0x4];
	void* pRenderable;
	const model_t *pModel;
	const matrix3x4_t *pModelToWorld;
	const matrix3x4_t *pLightingOffset;
	const Vector *pLightingOrigin;
	int flags;
	int entity_index;
	int skin;
	int body;
	int hitboxset;
	ModelInstanceHandle_t instance;
	ModelRenderInfo_t() {
		pModelToWorld = NULL;
		pLightingOffset = NULL;
		pLightingOrigin = NULL;
	}
};

typedef void* StudioDecalHandle_t;

struct studiohwdata_t;
struct DrawModelState_t {
	studiohdr_t*			m_pStudioHdr;
	studiohwdata_t*			m_pStudioHWData;
	IClientRenderable*		m_pRenderable;
	const matrix3x4_t		*m_pModelToWorld;
	StudioDecalHandle_t		m_decals;
	int						m_drawFlags;
	int						m_lod;
};

class IMatRenderContext;
enum OverrideType_t {
	OVERRIDE_NORMAL = 0,
	OVERRIDE_BUILD_SHADOWS,
	OVERRIDE_DEPTH_WRITE,
	OVERRIDE_SSAO_DEPTH_WRITE,
};

class IVModelRender {
public:
	VFUNC(ForcedMaterialOverride(IMaterial *mat = nullptr, OverrideType_t type = OVERRIDE_NORMAL), 1, void(__thiscall*)(void*, IMaterial*, OverrideType_t, int), mat, type, 0);
	VFUNC(IsForcedMaterialOverride(), 2, bool(__thiscall*)(void*));
	VFUNC(DrawModelExecute(IMatRenderContext* context, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* pCustomBoneToWorld = nullptr), 21, void(__thiscall*)(void*, IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t*), context, state, info, pCustomBoneToWorld);
};
extern IVModelRender* g_pModelRender;
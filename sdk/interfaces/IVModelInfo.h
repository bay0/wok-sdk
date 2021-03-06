#pragma once

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IMaterial;
class CUtlBuffer;
struct model_t;
struct studiohdr_t;
struct virtualmodel_t;
struct vcollide_t;
typedef unsigned short MDLHandle_t;

class Quaternion {
public:
	inline Quaternion() {}
	inline Quaternion(float ix, float iy, float iz, float iw) : x(ix), y(iy), z(iz), w(iw) {}

	inline void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f, float iw = 0.0f) { x = ix; y = iy; z = iz; w = iw; }

	float* Base() { return (float*)this; }
	const float* Base() const { return (float*)this; }

	float x, y, z, w;
};

class RadianEuler {
public:
	inline RadianEuler() {}
	inline RadianEuler(float X, float Y, float Z) { x = X; y = Y; z = Z; }
	inline RadianEuler(Quaternion const &q);	// evil auto type promotion!!!
	inline RadianEuler(QAngle const &angles);	// evil auto type promotion!!!

												// Initialization
	inline void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f) { x = ix; y = iy; z = iz; }

	//	conversion to qangle
	QAngle ToQAngle() const;
	bool IsValid() const;
	void Invalidate();

	inline float *Base() { return &x; }
	inline const float *Base() const { return &x; }

	// array access...
	float operator[](int i) const;
	float& operator[](int i);

	float x, y, z;
};

class ALIGN16 QuaternionAligned : public Quaternion {
public:
	inline QuaternionAligned() {};
	inline QuaternionAligned(float X, float Y, float Z, float W) {
		Init(X, Y, Z, W);
	}
public:
	explicit QuaternionAligned(const Quaternion &vOther) {
		Init(vOther.x, vOther.y, vOther.z, vOther.w);
	}

	QuaternionAligned& operator=(const Quaternion &vOther) {
		Init(vOther.x, vOther.y, vOther.z, vOther.w);
		return *this;
	}
};

struct mstudiobbox_t {
	int		bone;
	int		group;
	Vector	min;
	Vector	max;
	int		hitboxnameindex;
	Vector  rotation;
	float	radius;
	int		pad[4];

	const char* getHitboxName() {
		if (hitboxnameindex == 0)
			return "";

		return ((char*)this) + hitboxnameindex;
	}
};

struct mstudiohitboxset_t {
	int sznameindex;

	inline char * const pszName() const {
		return ((char*)this) + sznameindex;
	}

	int numhitboxes;
	int hitboxindex;

	inline mstudiobbox_t* pHitbox(int i) const {
		return (mstudiobbox_t*)(((byte*)this) + hitboxindex) + i;
	}
};

struct mstudiobone_t {
	int					sznameindex;
	inline char * const pszName() const { return ((char *)this) + sznameindex; }
	int		 			parent;		// parent bone
	int					bonecontroller[6];	// bone controller index, -1 == none

												// default values
	Vector				pos;
	Quaternion			quat;
	RadianEuler			rot;
	// compression scale
	Vector				posscale;
	Vector				rotscale;

	matrix3x4_t			poseToBone;
	Quaternion			qAlignment;
	int					flags;
	int					proctype;
	int					procindex;		// procedural rule
	mutable int			physicsbone;	// index into physically simulated bone
	inline void*		pProcedure() const { if (procindex == 0) return NULL; else return  (void *)(((byte *)this) + procindex); };
	int					surfacepropidx;	// index into string tablefor property name
	inline char* const  pszSurfaceProp() const { return ((char *)this) + surfacepropidx; }
	inline int			GetSurfaceProp() const { return surfacepropLookup; }

	int					contents;		// See BSPFlags.h for the contents flags
	int					surfacepropLookup;	// this index must be cached by the loader, not saved in the file
	int					unused[7];		// remove as appropriate
};

struct mstudiobonecontroller_t {
	int					bone;	// -1 == 0
	int					type;	// X, Y, Z, XR, YR, ZR, M
	float				start;
	float				end;
	int					rest;	// byte index value at rest
	int					inputfield;	// 0-3 user set controller, 4 mouth
	int					unused[8];
};

// attachment
struct mstudioattachment_t
{
	int					sznameindex;
	inline char * const pszName() const { return ((char *)this) + sznameindex; }
	unsigned int		flags;
	int					localbone;
	matrix3x4_t			local; // attachment point
	int					unused[8];
};

struct studiohdr_t {
	// Look up hitbox set by index
	mstudiohitboxset_t	*pHitboxSet(int i) const {
		Assert(i >= 0 && i < numhitboxsets);
		return (mstudiohitboxset_t *)(((byte *)this) + hitboxsetindex) + i;
	};

	// Calls through to hitbox to determine size of specified set
	inline mstudiobbox_t *pHitbox(int i, int set) const {
		mstudiohitboxset_t const *s = pHitboxSet(set);
		if (!s)
			return NULL;

		return s->pHitbox(i);
	};

	// Calls through to set to get hitbox count for set
	inline int iHitboxCount(int set) const {
		mstudiohitboxset_t const *s = pHitboxSet(set);
		if (!s)
			return 0;

		return s->numhitboxes;
	};
	inline mstudiobone_t *pBone(int i) const { Assert(i >= 0 && i < numbones); return (mstudiobone_t *)(((byte *)this) + boneindex) + i; };
	inline mstudioattachment_t	*pLocalAttachment(int i) const { Assert(i >= 0 && i < numlocalattachments); return (mstudioattachment_t *)(((byte *)this) + localattachmentindex) + i; };

	int id;
	int version;
	int checksum;
	char name[64];
	int length;

	Vector eyeposition;
	Vector illumposition;
	Vector hull_min;
	Vector hull_max;
	Vector view_bbmin;
	Vector view_bbmax;

	int flags;
	int numbones;
	int boneindex;

	int numbonecontrollers;
	int bonecontrollerindex;
	int numhitboxsets;
	int hitboxsetindex;

	int numlocalanim;
	int localanimindex;
	int numlocalseq;
	int localseqindex;

	mutable int activitylistversion;
	mutable int eventsindexed;

	int numtextures;
	int textureindex;
	int numcdtextures;
	int cdtextureindex;

	int numskinref;
	int numskinfamilies;
	int skinindex;

	int numbodyparts;
	int bodypartindex;
	int numlocalattachments;
	int localattachmentindex;

	int numlocalnodes;
	int localnodeindex;
	int localnodenameindex;

	int numflexdesc;
	int flexdescindex;
	int numflexcontrollers;
	int flexcontrollerindex;
	int numflexrules;
	int flexruleindex;
	int numikchains;
	int ikchainindex;
	int nummouths;
	int mouthindex;
	int numlocalposeparameters;
	int localposeparamindex;
	int surfacepropindex;

	int keyvalueindex;
	int keyvaluesize;

	int numlocalikautoplaylocks;
	int localikautoplaylockindex;

	float mass;
	int contents;
	int numincludemodels;
	int includemodelindex;

	mutable void *virtualModel;
	int szanimblocknameindex;

	int numanimblocks;
	int animblockindex;
	mutable void *animblockModel;
	int bonetablebynameindex;

	void *pVertexBase;
	void *pIndexBase;
	unsigned char constdirectionallightdot;
	unsigned char rootLOD;
	unsigned char numAllowedRootLODs;
	unsigned char unused[1];
	int unused4;
	int numflexcontrollerui;
	int flexcontrolleruiindex;
	int unused3[2];
	int studiohdr2index;
	int unused2[1];

	// ReSharper disable once CppPossiblyUninitializedMember
	studiohdr_t() {}
private:
	studiohdr_t(const studiohdr_t &vOther);

	friend struct virtualmodel_t;
};

class virtualgroup_t {
public:
	void *cache;
	const studiohdr_t *GetStudioHdr() const {
		return (studiohdr_t *)cache;
	}

	CUtlVector<int> boneMap;				// maps global bone to local bone
	CUtlVector<int> masterBone;			// maps local bone to global bone
	CUtlVector<int> masterSeq;			// maps local sequence to master sequence
	CUtlVector<int> masterAnim;			// maps local animation to master animation
	CUtlVector<int> masterAttachment;	// maps local attachment to global
	CUtlVector<int> masterPose;			// maps local pose parameter to global
	CUtlVector<int> masterNode;			// maps local transition nodes to global
};

struct virtualsequence_t {
	int	flags;
	int activity;
	int group;
	int index;
};

struct virtualgeneric_t {
	int group;
	int index;
};

struct virtualmodel_t {
	char pad_mutex[0x8];
	CUtlVector<virtualsequence_t> m_seq;
	CUtlVector<virtualgeneric_t> m_anim;
	CUtlVector<virtualgeneric_t> m_attachment;
	CUtlVector<virtualgeneric_t> m_pose;
	CUtlVector<virtualgroup_t> m_group;
	CUtlVector<virtualgeneric_t> m_node;
	CUtlVector<virtualgeneric_t> m_iklock;
	CUtlVector<unsigned short> m_autoplaySequences;
};

class CStudioHdr {
public:
	mutable studiohdr_t* m_pStudioHdr; //0x0000 
	mutable virtualmodel_t* m_pVModel; //0x0004 
	char				pad_unknown[0x4]; //0x0008 
	mutable CUtlVector<const studiohdr_t*> m_pStudioHdrCache; //0x000C 
	mutable int			m_nFrameUnlockCounter; //0x0020 
	int*				m_pFrameUnlockCounter; //0x0024 
	char				pad_mutex[0x8]; //0x0028 
	CUtlVector< int >	m_boneFlags; //0x0030
	CUtlVector< int >	m_boneParent; //0x0050 
	inline int			numbones() const { return m_pStudioHdr->numbones; };
	inline mstudiobone_t *pBone(int i) const { return m_pStudioHdr->pBone(i); };
	inline virtualmodel_t		*GetVirtualModel() const { return m_pVModel; };
	int	GetNumAttachments() const {
		if (m_pStudioHdr->numincludemodels == 0) {
			return m_pStudioHdr->numlocalattachments;
		}

		virtualmodel_t *pVModel = GetVirtualModel();
		Assert(pVModel);

		return pVModel->m_attachment.Count();
	}
	const studiohdr_t *GroupStudioHdr(int i) {
		const studiohdr_t *pStudioHdr = m_pStudioHdrCache[i];

		if (!pStudioHdr) {
			virtualgroup_t *pGroup = &m_pVModel->m_group[i];
			pStudioHdr = pGroup->GetStudioHdr();
		}

		Assert(pStudioHdr);
		return pStudioHdr;
	}
	const mstudioattachment_t &pAttachment(int i) {
		if (m_pVModel == NULL) {
			return *m_pStudioHdr->pLocalAttachment(i);
		}

		Assert(m_pVModel);

		const studiohdr_t *pStudioHdr = GroupStudioHdr(m_pVModel->m_attachment[i].group);

		return *pStudioHdr->pLocalAttachment(m_pVModel->m_attachment[i].index);
	}
	int	GetAttachmentBone(int i) {
		const mstudioattachment_t &attachment = pAttachment(i);

		// remap bone
		virtualmodel_t *pVModel = GetVirtualModel();
		if (pVModel) {
			virtualgroup_t *pGroup = &pVModel->m_group[pVModel->m_attachment[i].group];
			int iBone = pGroup->masterBone[attachment.localbone];
			if (iBone == -1)
				return 0;
			return iBone;
		}
		return attachment.localbone;
	}
}; //Size=0x0054


   //-----------------------------------------------------------------------------
   // Indicates the type of translucency of an unmodulated renderable
   //-----------------------------------------------------------------------------
enum RenderableTranslucencyType_t {
	RENDERABLE_IS_OPAQUE = 0,
	RENDERABLE_IS_TRANSLUCENT,
	RENDERABLE_IS_TWO_PASS,	// has both translucent and opaque sub-partsa
};


//-----------------------------------------------------------------------------
// Model info interface
//-----------------------------------------------------------------------------

class IVModelInfo {
public:
	virtual                                 ~IVModelInfo() {}
	virtual const model_t*                  GetModel(int modelindex) const = 0;
	virtual int                             GetModelIndex(const char *name) const = 0;
	virtual const char*                     GetModelName(const model_t *model) const = 0;
	virtual vcollide_t*                     GetVCollide(const model_t *model) const = 0;
	virtual vcollide_t*                     GetVCollide(int modelindex) const = 0;
	virtual void                            GetModelBounds(const model_t *model, Vector& mins, Vector& maxs) const = 0;
	virtual void                            GetModelRenderBounds(const model_t *model, Vector& mins, Vector& maxs) const = 0;
	virtual int                             GetModelFrameCount(const model_t *model) const = 0;
	virtual int                             GetModelType(const model_t *model) const = 0;
	virtual void*                           GetModelExtraData(const model_t *model) = 0;
	virtual bool                            ModelHasMaterialProxy(const model_t *model) const = 0;
	virtual bool                            IsTranslucent(model_t const* model) const = 0;
	virtual bool                            IsTranslucentTwoPass(const model_t *model) const = 0;
	virtual void                            unused0() {};
	virtual void							unused1() = 0;
	virtual void							unused2() = 0;
	virtual RenderableTranslucencyType_t    ComputeTranslucencyType(const model_t *model, int nSkin, int nBody) = 0;
	virtual int                             GetModelMaterialCount(const model_t* model) const = 0;
	virtual void                            GetModelMaterials(const model_t *model, int count, IMaterial** ppMaterial) = 0;
	virtual bool                            IsModelVertexLit(const model_t *model) const = 0;
	virtual const char*                     GetModelKeyValueText(const model_t *model) = 0;
	virtual bool                            GetModelKeyValue(const model_t *model, CUtlBuffer &buf) = 0;
	virtual float                           GetModelRadius(const model_t *model) = 0;
	virtual CStudioHdr*                     GetStudioHdr(MDLHandle_t handle) = 0;
	virtual const studiohdr_t*              FindModel(const studiohdr_t *pStudioHdr, void **cache, const char *modelname) const = 0;
	virtual const studiohdr_t*              FindModel(void *cache) const = 0;
	virtual virtualmodel_t*                 GetVirtualModel(const studiohdr_t *pStudioHdr) const = 0;
	virtual uint8_t*                        GetAnimBlock(const studiohdr_t *pStudioHdr, int iBlock) const = 0;
	virtual void                            GetModelMaterialColorAndLighting(const model_t *model, Vector const& origin, QAngle const& angles, CGameTrace* pTrace, Vector& lighting, Vector& matColor) = 0;
	virtual void                            GetIlluminationPoint(const model_t *model, IClientRenderable *pRenderable, Vector const& origin, QAngle const& angles, Vector* pLightingCenter) = 0;
	virtual int                             GetModelContents(int modelIndex) const = 0;
	virtual studiohdr_t*                    GetStudioModel(const model_t *mod) = 0;
	virtual int                             GetModelSpriteWidth(const model_t *model) const = 0;
	virtual int                             GetModelSpriteHeight(const model_t *model) const = 0;
	virtual void                            SetLevelScreenFadeRange(float flMinSize, float flMaxSize) = 0;
	virtual void                            GetLevelScreenFadeRange(float *pMinArea, float *pMaxArea) const = 0;
	virtual void                            SetViewScreenFadeRange(float flMinSize, float flMaxSize) = 0;
	virtual unsigned char                   ComputeLevelScreenFade(const Vector &vecAbsOrigin, float flRadius, float flFadeScale) const = 0;
	virtual unsigned char                   ComputeViewScreenFade(const Vector &vecAbsOrigin, float flRadius, float flFadeScale) const = 0;
	virtual int                             GetAutoplayList(const studiohdr_t *pStudioHdr, unsigned short **pAutoplayList) const = 0;
	virtual CPhysCollide*                   GetCollideForVirtualTerrain(int index) = 0;
	virtual bool                            IsUsingFBTexture(const model_t *model, int nSkin, int nBody, IClientRenderable* *pClientRenderable) const = 0;
	virtual const model_t*                  FindOrLoadModel(const char *name) const = 0;
	virtual MDLHandle_t                     GetCacheHandle(const model_t *model) const = 0;
	virtual int                             GetBrushModelPlaneCount(const model_t *model) const = 0;
	virtual void                            GetBrushModelPlane(const model_t *model, int nIndex, cplane_t &plane, Vector *pOrigin) const = 0;
	virtual int                             GetSurfacepropsForVirtualTerrain(int index) = 0;
	virtual bool                            UsesEnvCubemap(const model_t *model) const = 0;
	virtual bool                            UsesStaticLighting(const model_t *model) const = 0;
};
extern IVModelInfo* g_pModelInfo;
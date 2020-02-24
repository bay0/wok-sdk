#pragma once

struct surfacephysicsparams_t {
	float    friction;
	float    elasticity;
	float    density;
	float    thickness;
	float    dampening;
};

struct surfaceaudioparams_t {
	float    reflectivity;             
	float    hardnessFactor;           
	float    roughnessFactor;          
	float    roughThreshold;        
	float    hardThreshold;           
	float    hardVelocityThreshold;    
	float    highPitchOcclusion;       
	float    midPitchOcclusion;
	float    lowPitchOcclusion;
};

struct surfacesoundnames_t {
	unsigned short    walkStepLeft;
	unsigned short    walkStepRight;
	unsigned short	  runStepLeft;
	unsigned short	  runStepRight;
	unsigned short    impactSoft;
	unsigned short    impactHard;
	unsigned short    scrapeSmooth;
	unsigned short    scrapeRough;
	unsigned short    bulletImpact;
	unsigned short    rolling;
	unsigned short    breakSound;
	unsigned short    strainSound;
};

struct surfacegameprops_t {
public:
	float maxspeedfactor;
	float jumpfactor;
	float penetrationmodifier;
	float damagemodifier;
	uint16_t material;
	uint8_t climbable;
};

struct surfacedata_t {
	surfacephysicsparams_t physics;
	surfaceaudioparams_t audio;
	surfacesoundnames_t sounds;
	surfacegameprops_t game;
	char pad[48];
};

class IPhysicsSurfaceProps {
public:
	virtual ~IPhysicsSurfaceProps() {}
	virtual int ParseSurfaceData(const char *pFilename, const char *pTextfile) = 0;
	virtual int SurfacePropCount() const = 0;
	virtual int GetSurfaceIndex(const char *pSurfacePropName) const = 0;
	virtual void GetPhysicsProperties(int surfaceDataIndex, float *density, float *thickness, float *friction, float *elasticity) const = 0;
	virtual surfacedata_t* GetSurfaceData(int surfaceDataIndex) = 0;
	virtual const char* GetString(unsigned short stringTableIndex) const = 0;
	virtual const char* GetPropName(int surfaceDataIndex) const = 0;
	virtual void SetWorldMaterialIndexTable(int *pMapArray, int mapSize) = 0;
	virtual void GetPhysicsParameters(int surfaceDataIndex, surfacephysicsparams_t *pParamsOut) const = 0;
};
extern IPhysicsSurfaceProps* g_pSurfaceData;
/*******************************************************************************
*  Modulation.h
*
*******************************************************************************/

#ifndef __Modulation_h__
#define __Modulation_h__

enum ModulationType
{
	MODULATION_TYPE_PPM = 0,
	MODULATION_TYPE_INVERSE_PPM,
	MODULATION_TYPE_PCM,

    MODULATION_TYPE_MAX
};

extern const char* ModulationTypeName[MODULATION_TYPE_MAX];

void UseModulation(const ModulationType _modulation);

const char* GetCurrentModulationName();

#endif // __Modulation_h__
#pragma once

#include <stdint.h>

 struct BiosParamaterBlock 
 {
	uint8_t			m_OEMName[8];
	uint16_t		m_BytesPerSector;
	uint8_t			m_SectorsPerCluster;
	uint16_t		m_ReservedSectors;
	uint8_t			m_NumberOfFats;
	uint16_t		m_NumDirEntries;
	uint16_t		m_NumSectors;
	uint8_t			m_Media;
	uint16_t		m_SectorsPerFat;
	uint16_t		m_SectorsPerTrack;
	uint16_t		m_HeadsPerCylinder;
	uint32_t		m_HiddenSectors;
	uint32_t		m_LongSectors;

}__attribute((packed));

struct BiosParamaterBlockExt 
{
	uint32_t			m_SectorsPerFat32;
	uint16_t			m_Flags;
	uint16_t			m_Version;
	uint32_t			m_RootCluster;
	uint16_t			m_InfoCluster;
	uint16_t			m_BackupBoot;
	uint16_t			m_Reserved[6];

}__attribute((packed));

struct BootSector 
{
	uint8_t m_Ignore[3]; //first 3 bytes are ignored
	BiosParamaterBlock m_BiosParmataerBlock;
	BiosParamaterBlockExt m_BiosParamaterBlockExt;
	uint8_t Padding[448]; //pad to 512 bytes

}__attribute((packed));

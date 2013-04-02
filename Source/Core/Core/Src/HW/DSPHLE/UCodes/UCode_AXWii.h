// Copyright (C) 2003 Dolphin Project.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2.0.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License 2.0 for more details.

// A copy of the GPL 2.0 should have been included with the program.
// If not, see http://www.gnu.org/licenses/

// Official Git repository and contact information can be found at
// http://code.google.com/p/dolphin-emu/

#ifndef _UCODE_AXWII_H
#define _UCODE_AXWII_H

#include "UCode_AX.h"

class CUCode_AXWii : public CUCode_AX
{
public:
	CUCode_AXWii(DSPHLE *dsp_hle, u32 _CRC);
	virtual ~CUCode_AXWii();

	virtual void DoState(PointerWrap &p);

protected:
	// Additional AUX buffers
	int m_samples_auxC_left[32 * 3];
	int m_samples_auxC_right[32 * 3];
	int m_samples_auxC_surround[32 * 3];

	// Wiimote buffers
	int m_samples_wm0[6 * 3];
	int m_samples_aux0[6 * 3];
	int m_samples_wm1[6 * 3];
	int m_samples_aux1[6 * 3];
	int m_samples_wm2[6 * 3];
	int m_samples_aux2[6 * 3];
	int m_samples_wm3[6 * 3];
	int m_samples_aux3[6 * 3];

	// Are we implementing an old version of AXWii which still has updates?
	bool m_old_axwii;

	// Last volume values for MAIN and AUX. Used to generate volume ramps to
	// interpolate nicely between old and new volume values.
	u16 m_last_main_volume;
	u16 m_last_aux_volumes[3];

	// If needed, extract the updates related fields from a PB. We need to
	// reinject them afterwards so that the correct PB typs is written to RAM.
	bool ExtractUpdatesFields(AXPBWii& pb, u16* num_updates, u16* updates,
	                          u32* updates_addr);
	void ReinjectUpdatesFields(AXPBWii& pb, u16* num_updates, u32 updates_addr);

	// Convert a mixer_control bitfield to our internal representation for that
	// value. Required because that bitfield has a different meaning in some
	// versions of AX.
	AXMixControl ConvertMixerControl(u32 mixer_control);

	// Generate a volume ramp from vol1 to vol2, interpolating n volume values.
	// Uses floating point arithmetic, which isn't exactly what the UCode does,
	// but this gives better precision and nicer code.
	void GenerateVolumeRamp(u16* output, u16 vol1, u16 vol2, size_t nvals);

	virtual void HandleCommandList();

	void SetupProcessing(u32 init_addr);
	void AddToLR(u32 val_addr, bool neg);
	void AddSubToLR(u32 val_addr);
	void ProcessPBList(u32 pb_addr);
	void MixAUXSamples(int aux_id, u32 write_addr, u32 read_addr, u16 volume);
	void UploadAUXMixLRSC(int aux_id, u32* addresses, u16 volume);
	void OutputSamples(u32 lr_addr, u32 surround_addr, u16 volume,
	                   bool upload_auxc);
	void OutputWMSamples(u32* addresses);	// 4 addresses

private:
	enum CmdType
	{
		CMD_SETUP = 0x00,
		CMD_ADD_TO_LR = 0x01,
		CMD_SUB_TO_LR = 0x02,
		CMD_ADD_SUB_TO_LR = 0x03,
		CMD_PROCESS = 0x04,
		CMD_MIX_AUXA = 0x05,
		CMD_MIX_AUXB = 0x06,
		CMD_MIX_AUXC = 0x07,
		CMD_UPL_AUXA_MIX_LRSC = 0x08,
		CMD_UPL_AUXB_MIX_LRSC = 0x09,
		CMD_UNK_0A = 0x0A,
		CMD_OUTPUT = 0x0B,
		CMD_OUTPUT_DPL2 = 0x0C,
		CMD_WM_OUTPUT = 0x0D,
		CMD_END = 0x0E,
	};

	// A lot of these are similar to the new version, but there is an offset in
	// the command ids due to the PB_ADDR command (which was removed from the
	// new AXWii).
	enum CmdTypeOld
	{
		CMD_SETUP_OLD = 0x00,
		CMD_ADD_TO_LR_OLD = 0x01,
		CMD_SUB_TO_LR_OLD = 0x02,
		CMD_ADD_SUB_TO_LR_OLD = 0x03,
		CMD_PB_ADDR_OLD = 0x04,
		CMD_PROCESS_OLD = 0x05,
		CMD_MIX_AUXA_OLD = 0x06,
		CMD_MIX_AUXB_OLD = 0x07,
		CMD_MIX_AUXC_OLD = 0x08,
		CMD_UPL_AUXA_MIX_LRSC_OLD = 0x09,
		CMD_UPL_AUXB_MIX_LRSC_OLD = 0x0a,
		CMD_UNK_0B_OLD = 0x0B,
		CMD_OUTPUT_OLD = 0x0C,	// no volume!
		CMD_OUTPUT_DPL2_OLD = 0x0D,
		CMD_WM_OUTPUT_OLD = 0x0E,
		CMD_END_OLD = 0x0F
	};
};

#endif  // _UCODE_AXWII

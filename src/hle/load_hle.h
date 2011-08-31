/*  Copyright 2003-2005 Guillaume Duhamel
    Copyright 2004-2006 Theo Berkau
    Copyright 2006      Anders Montonen

    This file is part of Yabause.

    Yabause is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Yabause is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Yabause; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

void MappedMemoryWriteLong(u32 address, u32 data)
{
	CpuBusTransfer<kLong,kWrite>(address,data);
}

u32 MappedMemoryReadLong(u32 address)
{
	return CpuBusTransfer<kLong,kRead>(address);
}

void MappedMemoryWriteByte(u32 address, u8 data)
{
	CpuBusTransfer<kByte,kWrite>(address,data);
}

void YabauseSpeedySetup(void)
{
	u32 data;
	int i;

	if (true)//(yabsys.emulatebios = true)
		bios_hle.BiosInit();
	else
	{
		// Setup the vector table area, etc.(all bioses have it at 0x00000600-0x00000810)
		for (i = 0; i < 0x210; i+=4)
		{
			data = MappedMemoryReadLong(0x00000600+i);
			MappedMemoryWriteLong(0x06000000+i, data);
		}

		// Setup the bios function pointers, etc.(all bioses have it at 0x00000820-0x00001100)
		for (i = 0; i < 0x8E0; i+=4)
		{
			data = MappedMemoryReadLong(0x00000820+i);
			MappedMemoryWriteLong(0x06000220+i, data);
		}

		// I'm not sure this is really needed
		for (i = 0; i < 0x700; i+=4)
		{
			data = MappedMemoryReadLong(0x00001100+i);
			MappedMemoryWriteLong(0x06001100+i, data);
		}

		// Fix some spots in 0x06000210-0x0600032C area
		MappedMemoryWriteLong(0x06000234, 0x000002AC);
		MappedMemoryWriteLong(0x06000238, 0x000002BC);
		MappedMemoryWriteLong(0x0600023C, 0x00000350);
		MappedMemoryWriteLong(0x06000240, 0x32524459);
		MappedMemoryWriteLong(0x0600024C, 0x00000000);
		MappedMemoryWriteLong(0x06000268, MappedMemoryReadLong(0x00001344));
		MappedMemoryWriteLong(0x0600026C, MappedMemoryReadLong(0x00001348));
		MappedMemoryWriteLong(0x0600029C, MappedMemoryReadLong(0x00001354));
		MappedMemoryWriteLong(0x060002C4, MappedMemoryReadLong(0x00001104));
		MappedMemoryWriteLong(0x060002C8, MappedMemoryReadLong(0x00001108));
		MappedMemoryWriteLong(0x060002CC, MappedMemoryReadLong(0x0000110C));
		MappedMemoryWriteLong(0x060002D0, MappedMemoryReadLong(0x00001110));
		MappedMemoryWriteLong(0x060002D4, MappedMemoryReadLong(0x00001114));
		MappedMemoryWriteLong(0x060002D8, MappedMemoryReadLong(0x00001118));
		MappedMemoryWriteLong(0x060002DC, MappedMemoryReadLong(0x0000111C));
		MappedMemoryWriteLong(0x06000328, 0x000004C8);
		MappedMemoryWriteLong(0x0600032C, 0x00001800);

		// Fix SCU interrupts
		for (i = 0; i < 0x80; i+=4)
			MappedMemoryWriteLong(0x06000A00+i, 0x0600083C);
	}

	// Set the cpu's, etc. to sane states

	// Set CD block to a sane state
	cd_controller_hle.cs2_hle.Cs2Area->reg.HIRQ = 0xFC1;
	cd_controller_hle.cs2_hle.Cs2Area->isdiskchanged = 0;
	cd_controller_hle.cs2_hle.Cs2Area->reg.CR1 = (cd_controller_hle.cs2_hle.Cs2Area->status << 8) | ((cd_controller_hle.cs2_hle.Cs2Area->options & 0xF) << 4) | (cd_controller_hle.cs2_hle.Cs2Area->repcnt & 0xF);
	cd_controller_hle.cs2_hle.Cs2Area->reg.CR2 = (cd_controller_hle.cs2_hle.Cs2Area->ctrladdr << 8) | cd_controller_hle.cs2_hle.Cs2Area->track;
	cd_controller_hle.cs2_hle.Cs2Area->reg.CR3 = (cd_controller_hle.cs2_hle.Cs2Area->index << 8) | ((cd_controller_hle.cs2_hle.Cs2Area->FAD >> 16) & 0xFF);
	cd_controller_hle.cs2_hle.Cs2Area->reg.CR4 = (u16) cd_controller_hle.cs2_hle.Cs2Area->FAD; 
	cd_controller_hle.cs2_hle.Cs2Area->satauth = 4;

	// Set Master SH2 registers accordingly
	for (i = 0; i < 15; i++)
		master.regs.r[i] = 0x00000000;
	master.regs.r[15] = 0x06002000;
	master.regs.sr = 0x00000000;
	master.regs.gbr = 0x00000000;
	master.regs.vbr = 0x06000000;
	master.regs.mach = 0x00000000;
	master.regs.macl = 0x00000000;
	master.regs.pr = 0x00000000;
#if 0
	// Set SCU registers to sane states
	ScuRegs->D1AD = ScuRegs->D2AD = 0;
	ScuRegs->D0EN = 0x101;
	ScuRegs->IST = 0x2006;
	ScuRegs->AIACK = 0x1;
	ScuRegs->ASR0 = ScuRegs->ASR1 = 0x1FF01FF0;
	ScuRegs->AREF = 0x1F;
	ScuRegs->RSEL = 0x1;
#endif
	// Set SMPC registers to sane states
	smpc_hle.smpc_hle_impl.SmpcRegs->COMREG = 0x10;
	smpc_hle.smpc_hle_impl.SmpcInternalVars->resd = 0;

	vdp2.regs.tvmd.disp = true;
	vdp2.regs.tvstat.vblank = true;
	vdp2.regs.tvstat.odd = true;
	vdp2.regs.tvstat.exltfg = true;

	vdp2.regs.zoom[0].x.integer_part = 1;
	vdp2.regs.zoom[0].y.integer_part = 1;
	vdp2.regs.zoom[1].x.integer_part = 1;
	vdp2.regs.zoom[1].y.integer_part = 1;
#if 0
	// Set VDP1 registers to sane states
	Vdp1Regs->EDSR = 3;
	Vdp1Regs->localX = 160;
	Vdp1Regs->localY = 112;
	Vdp1Regs->systemclipX2 = 319;
	Vdp1Regs->systemclipY2 = 223;

	// Set VDP2 registers to sane states
	memset(Vdp2Regs, 0, sizeof(Vdp2));
	Vdp2Regs->TVMD = 0x8000;
	Vdp2Regs->TVSTAT = 0x020A;
	Vdp2Regs->CYCA0L = 0x0F44;
	Vdp2Regs->CYCA0U = 0xFFFF;
	Vdp2Regs->CYCA1L = 0xFFFF;
	Vdp2Regs->CYCA1U = 0xFFFF;
	Vdp2Regs->CYCB0L = 0xFFFF;
	Vdp2Regs->CYCB0U = 0xFFFF;
	Vdp2Regs->CYCB1L = 0xFFFF;
	Vdp2Regs->CYCB1U = 0xFFFF;
	Vdp2Regs->BGON = 0x0001;
	Vdp2Regs->PNCN0 = 0x8000;
	Vdp2Regs->MPABN0 = 0x0303;
	Vdp2Regs->MPCDN0 = 0x0303;
	Vdp2Regs->ZMXN0.all = 0x00010000;
	Vdp2Regs->ZMYN0.all = 0x00010000;
	Vdp2Regs->ZMXN1.all = 0x00010000;
	Vdp2Regs->ZMYN1.all = 0x00010000;
	Vdp2Regs->BKTAL = 0x4000;
	Vdp2Regs->SPCTL = 0x0020;
	Vdp2Regs->PRINA = 0x0007;
	Vdp2Regs->CLOFEN = 0x0001;
	Vdp2Regs->COAR = 0x0200;
	Vdp2Regs->COAG = 0x0200;
	Vdp2Regs->COAB = 0x0200;
	VIDCore->Vdp2SetResolution(Vdp2Regs->TVMD);
	VIDCore->Vdp2SetPriorityNBG0(Vdp2Regs->PRINA & 0x7);
	VIDCore->Vdp2SetPriorityNBG1((Vdp2Regs->PRINA >> 8) & 0x7);
	VIDCore->Vdp2SetPriorityNBG2(Vdp2Regs->PRINB & 0x7);
	VIDCore->Vdp2SetPriorityNBG3((Vdp2Regs->PRINB >> 8) & 0x7);
	VIDCore->Vdp2SetPriorityRBG0(Vdp2Regs->PRIR & 0x7);
#endif
}



int YabauseQuickLoadGame(void)
{
	Cs2Hle::partition_struct * lgpartition;
	u8 *buffer;
	u32 addr;
	u32 size;
	u32 blocks;
	unsigned int i, i2;
	Cs2Hle::dirrec_struct dirrec;

	cd_controller_hle.cs2_hle.Cs2Area->outconcddev = cd_controller_hle.cs2_hle.Cs2Area->filter + 0;
	cd_controller_hle.cs2_hle.Cs2Area->outconcddevnum = 0;

	// read in lba 0/FAD 150
	if ((lgpartition = cd_controller_hle.cs2_hle.Cs2ReadUnFilteredSector(150)) == NULL)
		return -1;

	// Make sure we're dealing with a saturn game
	buffer = lgpartition->block[lgpartition->numblocks - 1]->data;

	YabauseSpeedySetup();

	if (memcmp(buffer, "SEGA SEGASATURN", 15) == 0)
	{
		// figure out how many more sectors we need to read
		size = (buffer[0xE0] << 24) |
			(buffer[0xE1] << 16) |
			(buffer[0xE2] << 8) |
			buffer[0xE3];
		blocks = size >> 11;
		if ((size % 2048) != 0) 
			blocks++;


		// Figure out where to load the first program
		addr = (buffer[0xF0] << 24) |
			(buffer[0xF1] << 16) |
			(buffer[0xF2] << 8) |
			buffer[0xF3];

		// Free Block
		lgpartition->size = 0;
		cd_controller_hle.cs2_hle.Cs2FreeBlock(lgpartition->block[lgpartition->numblocks - 1]);
		lgpartition->blocknum[lgpartition->numblocks - 1] = 0xFF;
		lgpartition->numblocks = 0;

		// Copy over ip to 0x06002000
		for (i = 0; i < blocks; i++)
		{
			if ((lgpartition = cd_controller_hle.cs2_hle.Cs2ReadUnFilteredSector(150+i)) == NULL)
				return -1;

			buffer = lgpartition->block[lgpartition->numblocks - 1]->data;

			if (size >= 2048)
			{
				for (i2 = 0; i2 < 2048; i2++)
					MappedMemoryWriteByte(0x06002000 + (i * 0x800) + i2, buffer[i2]);
			}
			else
			{
				for (i2 = 0; i2 < size; i2++)
					MappedMemoryWriteByte(0x06002000 + (i * 0x800) + i2, buffer[i2]);
			}

			size -= 2048;

			// Free Block
			lgpartition->size = 0;
			cd_controller_hle.cs2_hle.Cs2FreeBlock(lgpartition->block[lgpartition->numblocks - 1]);
			lgpartition->blocknum[lgpartition->numblocks - 1] = 0xFF;
			lgpartition->numblocks = 0;
		}

		// Ok, now that we've loaded the ip, now it's time to load the
		// First Program

		// Figure out where the first program is located
		if ((lgpartition = cd_controller_hle.cs2_hle.Cs2ReadUnFilteredSector(166)) == NULL)
			return -1;

		// Figure out root directory's location

		// Retrieve directory record's lba
		cd_controller_hle.cs2_hle.Cs2CopyDirRecord(lgpartition->block[lgpartition->numblocks - 1]->data + 0x9C, &dirrec);

		// Free Block
		lgpartition->size = 0;
		cd_controller_hle.cs2_hle.Cs2FreeBlock(lgpartition->block[lgpartition->numblocks - 1]);
		lgpartition->blocknum[lgpartition->numblocks - 1] = 0xFF;
		lgpartition->numblocks = 0;

		// Now then, fetch the root directory's records
		if ((lgpartition = cd_controller_hle.cs2_hle.Cs2ReadUnFilteredSector(dirrec.lba+150)) == NULL)
			return -1;

		buffer = lgpartition->block[lgpartition->numblocks - 1]->data;

		// Skip the first two records, read in the last one
		for (i = 0; i < 3; i++)
		{
			cd_controller_hle.cs2_hle.Cs2CopyDirRecord(buffer, &dirrec);
			buffer += dirrec.recordsize;
		}

		size = dirrec.size;
		blocks = size >> 11;
		if ((dirrec.size % 2048) != 0)
			blocks++;

		// Free Block
		lgpartition->size = 0;
		cd_controller_hle.cs2_hle.Cs2FreeBlock(lgpartition->block[lgpartition->numblocks - 1]);
		lgpartition->blocknum[lgpartition->numblocks - 1] = 0xFF;
		lgpartition->numblocks = 0;

		// Copy over First Program to addr
		for (i = 0; i < blocks; i++)
		{
			if ((lgpartition = cd_controller_hle.cs2_hle.Cs2ReadUnFilteredSector(150+dirrec.lba+i)) == NULL)
				return -1;

			buffer = lgpartition->block[lgpartition->numblocks - 1]->data;

			if (size >= 2048)
			{
				for (i2 = 0; i2 < 2048; i2++)
					MappedMemoryWriteByte(addr + (i * 0x800) + i2, buffer[i2]);
			}
			else
			{
				for (i2 = 0; i2 < size; i2++)
					MappedMemoryWriteByte(addr + (i * 0x800) + i2, buffer[i2]);
			}

			size -= 2048;

			// Free Block
			lgpartition->size = 0;
			cd_controller_hle.cs2_hle.Cs2FreeBlock(lgpartition->block[lgpartition->numblocks - 1]);
			lgpartition->blocknum[lgpartition->numblocks - 1] = 0xFF;
			lgpartition->numblocks = 0;
		}

		// Now setup SH2 registers to start executing at ip code
		master.regs.pc = 0x06002E00;
	}
	else
	{
		// Ok, we're not. Time to bail!

		// Free Block
		lgpartition->size = 0;
		cd_controller_hle.cs2_hle.Cs2FreeBlock(lgpartition->block[lgpartition->numblocks - 1]);
		lgpartition->blocknum[lgpartition->numblocks - 1] = 0xFF;
		lgpartition->numblocks = 0;

		return -1;
	}

	return 0;
}
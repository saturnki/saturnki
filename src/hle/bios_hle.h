/*  Copyright 2006-2007 Theo Berkau

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

struct BiosHle
{

typedef struct
{
   char filename[12];
   char comment[11];
   u8 language;
   u8 year;
   u8 month;
   u8 day;
   u8 hour;
   u8 minute;
   u8 week;
   u32 datasize;
   u16 blocksize;
} saveinfo_struct;

typedef struct
{
   u8 id;
   char name[32];
} deviceinfo_struct;

Saturn &saturn;

BiosHle(Saturn &saturn_) : saturn(saturn_) {}
	u32 MappedMemoryReadLong(u32 address)
	{
		return saturn.CpuBusTransfer<kLong,kRead>(address);
		return 0;
	}
	void MappedMemoryWriteLong(u32 address, u32 data)
	{
		saturn.CpuBusTransfer<kLong,kWrite>(address,data);
	}
	u16 MappedMemoryReadWord(u32 address)
	{
		assert(false);
		return 0;
	}
	void MappedMemoryWriteWord(u32 address, u16 data)
	{
		assert(false);
	}
	u8 MappedMemoryReadByte(u32 address)
	{
		assert(false);
		return 0;
	}
	void MappedMemoryWriteByte(u32 address, u8 data)
	{
		assert(false);
	}
void FormatBackupRam(void *mem, u32 size)
{
	assert(false);
#if 0
   int i, i2;
   u32 i3;
   u8 header[32] = {
      0xFF, 'B', 0xFF, 'a', 0xFF, 'c', 0xFF, 'k',
      0xFF, 'U', 0xFF, 'p', 0xFF, 'R', 0xFF, 'a',
      0xFF, 'm', 0xFF, ' ', 0xFF, 'F', 0xFF, 'o',
      0xFF, 'r', 0xFF, 'm', 0xFF, 'a', 0xFF, 't'
   };

   // Fill in header
   for(i2 = 0; i2 < 4; i2++)
      for(i = 0; i < 32; i++)
         T1WriteByte(mem, (i2 * 32) + i, header[i]);

   // Clear the rest
   for(i3 = 0x80; i3 < size; i3+=2)
   {
      T1WriteByte(mem, i3, 0xFF);
      T1WriteByte(mem, i3+1, 0x00);
   }
#endif
}

	u32 interruptlist[2][0x80];

	void SmpcCKCHG320();
	void SmpcCKCHG352();

	//////////////////////////////////////////////////////////////////////////////

	void BiosInit(void)
	{
		int i;

		// Setup vectors
		MappedMemoryWriteLong(0x06000600, 0x002B0009); // rte, nop
		MappedMemoryWriteLong(0x06000604, 0xE0F0600C); // mov #0xF0, r0; extu.b r0, r0
		MappedMemoryWriteLong(0x06000608, 0x400E8BFE); // ldc r0, sr; bf
		MappedMemoryWriteLong(0x0600060C, 0x00090009); // nop
		MappedMemoryWriteLong(0x06000610, 0x000B0009); // rts, nop

		for (i = 0; i < 0x200; i+=4)
		{
			MappedMemoryWriteLong(0x06000000+i, 0x06000600);
			MappedMemoryWriteLong(0x06000400+i, 0x06000600);
			interruptlist[0][i >> 2] = 0x06000600;
			interruptlist[1][i >> 2] = 0x06000600;
		}

		MappedMemoryWriteLong(0x06000010, 0x06000604);
		MappedMemoryWriteLong(0x06000018, 0x06000604);
		MappedMemoryWriteLong(0x06000024, 0x06000604);
		MappedMemoryWriteLong(0x06000028, 0x06000604);
		interruptlist[0][4] = 0x06000604;
		interruptlist[0][6] = 0x06000604;
		interruptlist[0][9] = 0x06000604;
		interruptlist[0][10] = 0x06000604;

		MappedMemoryWriteLong(0x06000410, 0x06000604);
		MappedMemoryWriteLong(0x06000418, 0x06000604);
		MappedMemoryWriteLong(0x06000424, 0x06000604);
		MappedMemoryWriteLong(0x06000428, 0x06000604);
		interruptlist[1][4] = 0x06000604;
		interruptlist[1][6] = 0x06000604;
		interruptlist[1][9] = 0x06000604;
		interruptlist[1][10] = 0x06000604;

		// Scu Interrupts
		for (i = 0; i < 0x38; i+=4)
		{
			MappedMemoryWriteLong(0x06000100+i, 0x00000400+i);
			interruptlist[0][0x40+(i >> 2)] = 0x00000400+i;
		}

		for (i = 0; i < 0x40; i+=4)
		{
			MappedMemoryWriteLong(0x06000140+i, 0x00000440+i);
			interruptlist[0][0x50+(i >> 2)] = 0x00000440+i;
		}

		for (i = 0; i < 0x100; i+=4)
			MappedMemoryWriteLong(0x06000A00+i, 0x06000610);

		// Setup Bios Functions
		MappedMemoryWriteLong(0x06000210, 0x00000210);
		MappedMemoryWriteLong(0x0600026C, 0x0000026C);
		MappedMemoryWriteLong(0x06000274, 0x00000274);
		MappedMemoryWriteLong(0x06000280, 0x00000280);
		MappedMemoryWriteLong(0x0600029C, 0x0000029C);
		MappedMemoryWriteLong(0x060002DC, 0x000002DC);
		MappedMemoryWriteLong(0x06000300, 0x00000300);
		MappedMemoryWriteLong(0x06000304, 0x00000304);
		MappedMemoryWriteLong(0x06000310, 0x00000310);
		MappedMemoryWriteLong(0x06000314, 0x00000314);
		MappedMemoryWriteLong(0x06000320, 0x00000320);
		MappedMemoryWriteLong(0x06000324, 0x00000000);
		MappedMemoryWriteLong(0x06000330, 0x00000330);
		MappedMemoryWriteLong(0x06000334, 0x00000334);
		MappedMemoryWriteLong(0x06000340, 0x00000340);
		MappedMemoryWriteLong(0x06000344, 0x00000344);
		MappedMemoryWriteLong(0x06000348, 0xFFFFFFFF);
		MappedMemoryWriteLong(0x06000354, 0x00000000);
		MappedMemoryWriteLong(0x06000358, 0x00000358);
	}

	//////////////////////////////////////////////////////////////////////////////

	void FASTCALL BiosSetScuInterrupt(Sh2 & sh)
	{
		//   LOG("BiosSetScuInterrupt. vector = %02X, func = %08X\n", sh.regs.r[4], sh.regs.r[5]);

		if (sh.regs.r[5] == 0)
		{
			MappedMemoryWriteLong(0x06000900+(sh.regs.r[4] << 2), 0x06000610);      
			sh.regs.cycles += 8;
		}
		else
		{
			MappedMemoryWriteLong(0x06000900+(sh.regs.r[4] << 2), sh.regs.r[5]);
			sh.regs.cycles += 9;
		}

		sh.regs.pc = sh.regs.pr;
	}

	//////////////////////////////////////////////////////////////////////////////

	void FASTCALL BiosGetScuInterrupt(Sh2 &sh)
	{
		// check me
		//   LOG("BiosGetScuInterrupt\n"); 

		sh.regs.r[0] = MappedMemoryReadLong(0x06000900+(sh.regs.r[4] << 2));
		sh.regs.cycles += 5;

		sh.regs.pc = sh.regs.pr;
	}

	//////////////////////////////////////////////////////////////////////////////

	void FASTCALL BiosSetSh2Interrupt(Sh2 &sh)
	{
		//   LOG("BiosSetSh2Interrupt\n");

		if (sh.regs.r[5] == 0)
		{            
			MappedMemoryWriteLong(sh.regs.vbr+(sh.regs.r[4] << 2), interruptlist[!sh.is_master][sh.regs.r[4]]);
			sh.regs.cycles += 8;
		}
		else
		{
			MappedMemoryWriteLong(sh.regs.vbr+(sh.regs.r[4] << 2), sh.regs.r[5]);
			sh.regs.cycles += 9;
		}

		sh.regs.pc = sh.regs.pr;
	}

	//////////////////////////////////////////////////////////////////////////////

	void FASTCALL BiosGetSh2Interrupt(Sh2 &sh)
	{
		// check me
		//   LOG("BiosGetSh2Interrupt\n");

		sh.regs.r[0] = MappedMemoryReadLong(sh.regs.vbr+(sh.regs.r[4] << 2));
		sh.regs.cycles += 5;

		sh.regs.pc = sh.regs.pr;
	}

	//////////////////////////////////////////////////////////////////////////////

	void FASTCALL BiosSetScuInterruptMask(Sh2 &sh)
	{
		// check me
		LOG("BiosSetScuInterruptMask\n");

		MappedMemoryWriteLong(0x06000348, sh.regs.r[4]);
		MappedMemoryWriteLong(0x25FE00A0, sh.regs.r[4]); // Interrupt Mask Register

		if (!(sh.regs.r[4] & 0x8000)) // double check this
			MappedMemoryWriteLong(0x25FE00A8, 1); // A-bus Interrupt Acknowledge

		sh.regs.cycles += 17;

		sh.regs.pc = sh.regs.pr;
	}

	//////////////////////////////////////////////////////////////////////////////

	void FASTCALL BiosChangeScuInterruptMask(Sh2 &sh)
	{
		u32 newmask;

		//   LOG("BiosChangeScuInterruptMask\n");

		// Read Stored Scu Interrupt Mask, AND it by R4, OR it by R5, then put it back
		newmask = (MappedMemoryReadLong(0x06000348) & sh.regs.r[4]) | sh.regs.r[5];
		MappedMemoryWriteLong(0x06000348, newmask);
		MappedMemoryWriteLong(0x25FE00A0, newmask); // Interrupt Mask Register
		MappedMemoryWriteLong(0x25FE00A4, (u32)(s16)sh.regs.r[4]); // Interrupt Status Register

		if (!(sh.regs.r[4] & 0x8000)) // double check this
			MappedMemoryWriteLong(0x25FE00A8, 1); // A-bus Interrupt Acknowledge

		sh.regs.cycles += 20;

		sh.regs.pc = sh.regs.pr;
	}

	//////////////////////////////////////////////////////////////////////////////

	void FASTCALL BiosCDINIT2(Sh2 &sh)
	{
		sh.regs.pc = sh.regs.pr;
	}

	//////////////////////////////////////////////////////////////////////////////

	void FASTCALL BiosCDINIT1(Sh2 &sh)
	{
		sh.regs.pc = sh.regs.pr;
	}

	//////////////////////////////////////////////////////////////////////////////

	void FASTCALL BiosGetSemaphore(Sh2 &sh)
	{
		u8 temp;

		// check me
		LOG("BiosGetSemaphore\n");

		if ((temp = MappedMemoryReadByte(0x06000B00 + sh.regs.r[4])) == 0)
			sh.regs.r[0] = 1;
		else
			sh.regs.r[0] = 0;

		temp |= 0x80;
		MappedMemoryWriteByte(0x06000B00 + sh.regs.r[4], temp);

		sh.regs.cycles += 11;
		sh.regs.pc = sh.regs.pr;
	}

	//////////////////////////////////////////////////////////////////////////////

	void FASTCALL BiosClearSemaphore(Sh2 &sh)
	{
		// check me
		LOG("BiosClearSemaphore\n");

		MappedMemoryWriteByte(0x06000B00 + sh.regs.r[4], 0);

		sh.regs.cycles += 5;
		sh.regs.pc = sh.regs.pr;
	}

	//////////////////////////////////////////////////////////////////////////////

	void FASTCALL BiosChangeSystemClock(Sh2 &sh)
	{
		LOG("BiosChangeSystemClock\n");

		MappedMemoryWriteLong(0x06000324, sh.regs.r[4]);

		if (sh.regs.r[4] == 0)
			saturn.smpc_hle.smpc_hle_impl.SmpcCKCHG320(saturn);
			//SmpcCKCHG320();
		else
			saturn.smpc_hle.smpc_hle_impl.SmpcCKCHG352(saturn);
			//SmpcCKCHG352();

		sh.regs.pc = sh.regs.pr;
	}

	//////////////////////////////////////////////////////////////////////////////

	void FASTCALL BiosChangeScuInterruptPriority(Sh2 &sh)
	{
		int i;

		// check me
		//   LOG("BiosChangeScuInterruptPriority\n");

		for (i = 0; i < 0x20; i++)
		{
			scumasklist[i] = MappedMemoryReadLong(sh.regs.r[4]+(i << 2));
			sh2masklist[i] = (scumasklist[i] >> 16);
			if (scumasklist[i] & 0x8000)
				scumasklist[i] |= 0xFFFF0000;
			else
				scumasklist[i] &= 0x0000FFFF;
		}

		sh.regs.cycles += 186;
		sh.regs.pc = sh.regs.pr;
	}

	//////////////////////////////////////////////////////////////////////////////

	void FASTCALL BiosExecuteCDPlayer(Sh2 &sh)
	{
		LOG("BiosExecuteCDPlayer\n");

		sh.regs.pc = sh.regs.pr;
	}

	//////////////////////////////////////////////////////////////////////////////

	void FASTCALL BiosPowerOnMemoryClear(Sh2 &sh)
	{
		LOG("BiosPowerOnMemoryClear\n");

		sh.regs.pc = sh.regs.pr;
	}

	//////////////////////////////////////////////////////////////////////////////

	void FASTCALL BiosCheckMPEGCard(Sh2 &sh)
	{
		LOG("BiosCheckMPEGCard\n");

		sh.regs.pc = sh.regs.pr;
	}

	//////////////////////////////////////////////////////////////////////////////

	u32 GetDeviceStats(u32 device, u32 *size, u32 *addr, u32 *blocksize)
	{
		switch(device)
		{
		case 0:
			*addr = 0x00180000;
			*size = 0x8000;
			*blocksize = 0x40;
			return 0;
		case 1:
			assert(false);
			return 0;
#if 0
			if ((CartridgeArea->cartid & 0xF0) == 0x20)
			{
				*addr = 0x04000000;
				*size = 0x40000 << (CartridgeArea->cartid & 0x0F);
				if (CartridgeArea->cartid == 0x24)
					*blocksize = 0x400;
				else
					*blocksize = 0x200;

				return 0;
			}
			else
				return 1;
#endif
		default:
			*addr = 0;
			*size = 0;
			*blocksize = 0;
			return 1;
		}

		return 1;
	}

	//////////////////////////////////////////////////////////////////////////////

	int CheckHeader(UNUSED u32 device)
	{
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////////

	int CalcSaveSize(u32 tableaddr, int blocksize)
	{
		int numblocks=0;

		// Now figure out how many blocks this save is
		for(;;)
		{
			u16 block;
			block = (MappedMemoryReadByte(tableaddr) << 8) | MappedMemoryReadByte(tableaddr + 2);
			if (block == 0)
				break;
			tableaddr += 4;
			if (((tableaddr-1) & ((blocksize << 1) - 1)) == 0)
				tableaddr += 8;
			numblocks++;
		}

		return numblocks;
	}

	//////////////////////////////////////////////////////////////////////////////

	u32 GetFreeSpace_(u32 device, u32 size, u32 addr, u32 blocksize)
	{
		u32 i;
		u32 usedblocks=0;

		for (i = ((2 * blocksize) << 1); i < (size << 1); i += (blocksize << 1))
		{
			// Find a block with the start of a save
			if (((s8)MappedMemoryReadByte(addr + i + 1)) < 0)
			{
				// Now figure out how many blocks this save is
				usedblocks += (CalcSaveSize(addr+i+0x45, blocksize) + 1);
			}
		}

		return ((size / blocksize) - 2 - usedblocks);
	}

	//////////////////////////////////////////////////////////////////////////////

	u32 FindSave(UNUSED u32 device, u32 stringaddr, u32 blockoffset, u32 size, u32 addr, u32 blocksize)
	{
		u32 i;

		for (i = ((blockoffset * blocksize) << 1); i < (size << 1); i += (blocksize << 1))
		{
			// Find a block with the start of a save
			if (((s8)MappedMemoryReadByte(addr + i + 1)) < 0)
			{
				int i3;

				// See if string matches, or if there's no string to check, just copy
				// the data over
				for (i3 = 0; i3 < 11; i3++)
				{            
					u8 data = MappedMemoryReadByte(stringaddr+i3);

					if (MappedMemoryReadByte(addr+i+0x9+(i3*2)) != data)
					{
						if (data == 0)
							// There's no string to match
							return ((i / blocksize) >> 1);
						else
							// No Match, move onto the next block
							i3 = 12;
					}
					else
					{
						// Match
						if (i3 == 10 || data == 0)
							return ((i / blocksize) >> 1);
					}
				}
			}
		}

		return 0;
	}

	//////////////////////////////////////////////////////////////////////////////

	u32 FindSave2(UNUSED u32 device, const char *string, u32 blockoffset, u32 size, u32 addr, u32 blocksize)
	{
		u32 i;

		for (i = ((blockoffset * blocksize) << 1); i < (size << 1); i += (blocksize << 1))
		{
			// Find a block with the start of a save
			if (((s8)MappedMemoryReadByte(addr + i + 1)) < 0)
			{
				int i3;

				// See if string matches, or if there's no string to check, just copy
				// the data over
				for (i3 = 0; i3 < 11; i3++)
				{            
					if (MappedMemoryReadByte(addr+i+0x9+(i3*2)) != string[i3])
					{
						if (string[i3] == 0)
							// There's no string to match
							return ((i / blocksize) >> 1);
						else
							// No Match, move onto the next block
							i3 = 12;
					}
					else
					{
						// Match
						if (i3 == 10 || string[i3] == 0)
							return ((i / blocksize) >> 1);
					}
				}
			}
		}

		return 0;
	}

	//////////////////////////////////////////////////////////////////////////////

	void DeleteSave(u32 addr, u32 blockoffset, u32 blocksize)
	{
		MappedMemoryWriteByte(addr + (blockoffset * blocksize * 2) + 0x1, 0x00);
	}

	//////////////////////////////////////////////////////////////////////////////

	u16 *GetFreeBlocks(u32 addr, u32 blocksize, u32 numblocks, u32 size)
	{
		u8 *blocktbl;
		u16 *freetbl;
		u32 tableaddr;
		u32 i;
		u32 blockcount=0;

		// Create a table that tells us which blocks are free and used
		if ((blocktbl = (u8 *)malloc(sizeof(u8) * (size / blocksize))) == NULL)
			return NULL;

		memset(blocktbl, 0, (size / blocksize));

		for (i = ((2 * blocksize) << 1); i < (size << 1); i += (blocksize << 1))
		{
			// Find a block with the start of a save
			if (((s8)MappedMemoryReadByte(addr + i + 1)) < 0)
			{
				tableaddr = addr+i+0x45;
				blocktbl[i / (blocksize << 1)] = 1;

				// Now let's figure out which blocks are used
				for(;;)
				{
					u16 block;
					block = (MappedMemoryReadByte(tableaddr) << 8) | MappedMemoryReadByte(tableaddr + 2);
					if (block == 0)
						break;
					tableaddr += 4;
					if (((tableaddr-1) & ((blocksize << 1) - 1)) == 0)
						tableaddr += 8;
					// block is used
					blocktbl[block] = 1;
				}
			}
		}

		if ((freetbl = (u16 *)malloc(sizeof(u16) * numblocks)) == NULL)
		{
			free(blocktbl);
			return NULL;
		}

		// Find some free blocks for us to use
		for (i = 2; i < (size / blocksize); i++)
		{
			if (blocktbl[i] == 0)
			{
				freetbl[blockcount] = (u16)i;
				blockcount++;

				if (blockcount >= numblocks)
					break;
			}
		}

		// Ok, we're all done
		free(blocktbl);

		return freetbl;
	}

	//////////////////////////////////////////////////////////////////////////////

	u16 *ReadBlockTable(u32 addr, u32 *tableaddr, int block, int blocksize, int *numblocks, int *blocksread)
	{
		u16 *blocktbl;
		int i=0;

		tableaddr[0] = addr + (block * blocksize * 2) + 0x45;
		blocksread[0]=0;

		// First of all figure out how large of buffer we need
		numblocks[0] = CalcSaveSize(tableaddr[0], blocksize);

		// Allocate buffer
		if ((blocktbl = (u16 *)malloc(sizeof(u16) * numblocks[0])) == NULL)
			return NULL;

		// Now read in the table
		for(i = 0; i < numblocks[0]; i++)
		{
			u16 block;
			block = (MappedMemoryReadByte(tableaddr[0]) << 8) | MappedMemoryReadByte(tableaddr[0] + 2);
			tableaddr[0] += 4;

			if (((tableaddr[0]-1) & ((blocksize << 1) - 1)) == 0)
			{
				tableaddr[0] = addr + (blocktbl[blocksread[0]] * blocksize * 2) + 9;
				blocksread[0]++;
			}
			blocktbl[i] = block;
		}

		tableaddr[0] += 4;

		return blocktbl;
	}

	//////////////////////////////////////////////////////////////////////////////

	void FASTCALL BiosBUPInit(Sh2 &sh)
	{
		//   LOG("BiosBUPInit. arg1 = %08X, arg2 = %08X, arg3 = %08X\n", sh.regs.r[4], sh.regs.r[5], sh.regs.r[6]);

		// Setup Function table
		MappedMemoryWriteLong(0x06000354, sh.regs.r[5]);
		MappedMemoryWriteLong(sh.regs.r[5]+0x00, 0x00000380);
		MappedMemoryWriteLong(sh.regs.r[5]+0x04, 0x00000384);
		MappedMemoryWriteLong(sh.regs.r[5]+0x08, 0x00000388);
		MappedMemoryWriteLong(sh.regs.r[5]+0x0C, 0x0000038C);
		MappedMemoryWriteLong(sh.regs.r[5]+0x10, 0x00000390);
		MappedMemoryWriteLong(sh.regs.r[5]+0x14, 0x00000394);
		MappedMemoryWriteLong(sh.regs.r[5]+0x18, 0x00000398);
		MappedMemoryWriteLong(sh.regs.r[5]+0x1C, 0x0000039C);
		MappedMemoryWriteLong(sh.regs.r[5]+0x20, 0x000003A0);
		MappedMemoryWriteLong(sh.regs.r[5]+0x24, 0x000003A4);
		MappedMemoryWriteLong(sh.regs.r[5]+0x28, 0x000003A8);
		MappedMemoryWriteLong(sh.regs.r[5]+0x2C, 0x000003AC);

		// Setup Device list

		// First Device
		MappedMemoryWriteWord(sh.regs.r[6], 1); // ID
		MappedMemoryWriteWord(sh.regs.r[6]+0x2, 1); // Number of partitions
#if 0
		// Second Device
		if ((CartridgeArea->cartid & 0xF0) == 0x20)
		{
			MappedMemoryWriteWord(sh.regs.r[6]+0x4, 2); // ID
			MappedMemoryWriteWord(sh.regs.r[6]+0x6, 1); // Number of partitions
		}
		else
		{
			MappedMemoryWriteWord(sh.regs.r[6]+0x4, 0); // ID
			MappedMemoryWriteWord(sh.regs.r[6]+0x6, 0); // Number of partitions
		}
#endif
		printf("second device is  broken\n");
		// Third Device
		MappedMemoryWriteWord(sh.regs.r[6]+0x08, 0); // ID
		MappedMemoryWriteWord(sh.regs.r[6]+0x0A, 0); // Number of partitions

		// cycles need to be incremented

		sh.regs.pc = sh.regs.pr;
	}

	//////////////////////////////////////////////////////////////////////////////

	void FASTCALL BiosBUPSelectPartition(Sh2 &sh)
	{
		LOG("BiosBUPSelectPartition. PR = %08X\n", sh.regs.pr);

		sh.regs.r[0] = 0; // returns 0 if there's no error
		sh.regs.pc = sh.regs.pr;
	}

	//////////////////////////////////////////////////////////////////////////////

	void FASTCALL BiosBUPFormat(Sh2 &sh)
	{
		//   LOG("BiosBUPFormat. PR = %08X\n", sh.regs.pr);

		BupFormat(sh.regs.r[4]);

		sh.regs.r[0] = 0; // returns 0 if there's no error
		sh.regs.pc = sh.regs.pr;
	}

	//////////////////////////////////////////////////////////////////////////////

	void FASTCALL BiosBUPStatus(Sh2 &sh)
	{
		u32 size;
		u32 addr;
		u32 blocksize;
		u32 ret;
		u32 freeblocks=0;

		//   LOG("BiosBUPStatus. arg1 = %d, arg2 = %d, arg3 = %08X, PR = %08X\n", sh.regs.r[4], sh.regs.r[5], sh.regs.r[6], sh.regs.pr);

		// Fill in status variables
		ret = GetDeviceStats(sh.regs.r[4], &size, &addr, &blocksize);

		// Make sure there's a proper header, and return if there's any other errors
		if (ret == 1 || CheckHeader(sh.regs.r[4]) != 0)
		{
			// Error
			sh.regs.r[0] = ret;
			sh.regs.pc = sh.regs.pr;
			return;
		}
		assert(false);
//		freeblocks = GetFreeSpace(sh.regs.r[4], size, addr, blocksize);

		MappedMemoryWriteLong(sh.regs.r[6], size); // Size of Backup Ram (in bytes)
		MappedMemoryWriteLong(sh.regs.r[6]+0x4, size / blocksize); // Size of Backup Ram (in blocks)
		MappedMemoryWriteLong(sh.regs.r[6]+0x8, blocksize); // Size of block
		MappedMemoryWriteLong(sh.regs.r[6]+0xC, ((blocksize - 6) * freeblocks) - 30); // Free space(in bytes)
		MappedMemoryWriteLong(sh.regs.r[6]+0x10, freeblocks); // Free space(in blocks)
		MappedMemoryWriteLong(sh.regs.r[6]+0x14, freeblocks); // Not sure, but seems to be the same as Free Space(in blocks)

		// cycles need to be incremented

		sh.regs.r[0] = ret; // returns 0 if there's no error
		sh.regs.pc = sh.regs.pr;
	}

	//////////////////////////////////////////////////////////////////////////////

	void FASTCALL BiosBUPWrite(Sh2 &sh)
	{
		u32 size;
		u32 addr;
		u32 blocksize;
		u32 block;
		u32 ret;
		u32 savesize;
		u16 *blocktbl;
		u32 workaddr;
		u32 blockswritten=0;
		u32 datasize;
		u32 i;

		LOG("BiosBUPWrite. arg1 = %d, arg2 = %08X, arg3 = %08X, arg4 = %d, PR = %08X\n", sh.regs.r[4], sh.regs.r[5], sh.regs.r[6], sh.regs.r[7], sh.regs.pr);

		// Fill in status variables
		ret = GetDeviceStats(sh.regs.r[4], &size, &addr, &blocksize);
		if (ret == 1)
		{
			// Error
			sh.regs.r[0] = ret;
			sh.regs.pc = sh.regs.pr;
			return;
		}

		// See if save exists already
		if ((block = FindSave(sh.regs.r[4], sh.regs.r[5], 2, size, addr, blocksize)) != 0)
		{
			// save exists

			// Should we be overwriting it?
			if (sh.regs.r[7] != 0)
			{
				// Nope, let's bail instead
				sh.regs.r[0] = 6;
				sh.regs.pc = sh.regs.pr;
				return;
			}

			// Delete old save
			DeleteSave(addr, block, blocksize);
		}

		// Let's figure out how many blocks will be needed for the save
		datasize = MappedMemoryReadLong(sh.regs.r[5]+0x1C);
		savesize = (datasize + 0x1D) / (blocksize - 6);
		if ((datasize + 0x1D) % (blocksize - 6))
			savesize++;

		assert(false);
#if 0
		// Will it blend? Err... fit
		if (savesize > GetFreeSpace(sh.regs.r[4], size, addr, blocksize))
		{
			// Nope, time to bail
			sh.regs.r[0] = 4;
			sh.regs.pc = sh.regs.pr;
			return;
		}
#endif
		// Find free blocks for the save
		if ((blocktbl = GetFreeBlocks(addr, blocksize, savesize, size)) == NULL)
		{
			// Just return an error that might make sense
			sh.regs.r[0] = 8;
			sh.regs.pc = sh.regs.pr;
			return;
		}

		// Create save
		workaddr = addr + (blocktbl[0] * blocksize * 2);

		MappedMemoryWriteByte(workaddr+0x1, 0x80);

		// Copy over filename
		for (i = workaddr+0x9; i < ((workaddr+0x9) + (11 * 2)); i+=2)
		{
			MappedMemoryWriteByte(i, MappedMemoryReadByte(sh.regs.r[5]));
			sh.regs.r[5]++;
		}

		sh.regs.r[5]++;

		// Copy over comment
		for (i = workaddr+0x21; i < ((workaddr+0x21) + (10 * 2)); i+=2)
		{
			MappedMemoryWriteByte(i, MappedMemoryReadByte(sh.regs.r[5]));
			sh.regs.r[5]++;
		}

		// Copy over language
		MappedMemoryWriteByte(workaddr+0x1F, MappedMemoryReadByte(sh.regs.r[5]));
		sh.regs.r[5]++;

		sh.regs.r[5]++;

		// Copy over date
		for (i = workaddr+0x35; i < ((workaddr+0x35) + (4 * 2)); i+=2)
		{
			MappedMemoryWriteByte(i, MappedMemoryReadByte(sh.regs.r[5]));
			sh.regs.r[5]++;
		}

		// Copy over data size
		for (i = workaddr+0x3D; i < ((workaddr+0x3D) + (4 * 2)); i+=2)
		{
			MappedMemoryWriteByte(i, MappedMemoryReadByte(sh.regs.r[5]));
			sh.regs.r[5]++;
		}

		// write the block table
		workaddr += 0x45;

		for (i = 1; i < savesize; i++)
		{
			MappedMemoryWriteByte(workaddr, (u8)(blocktbl[i] >> 8));
			workaddr+=2;
			MappedMemoryWriteByte(workaddr, (u8)blocktbl[i]);
			workaddr+=2;

			if (((workaddr-1) & ((blocksize << 1) - 1)) == 0)
			{
				// Next block
				blockswritten++;
				workaddr = addr + (blocktbl[blockswritten] * blocksize * 2) + 9;
			}
		}

		// Write 2 blank bytes so we now how large the table size is next time
		MappedMemoryWriteByte(workaddr, 0);
		workaddr+=2;
		MappedMemoryWriteByte(workaddr, 0);
		workaddr+=2;

		// Lastly, write the actual save data
		while (datasize > 0)
		{
			MappedMemoryWriteByte(workaddr, MappedMemoryReadByte(sh.regs.r[6]));
			datasize--;
			sh.regs.r[6]++;
			workaddr+=2;

			if (((workaddr-1) & ((blocksize << 1) - 1)) == 0)
			{
				// Next block
				blockswritten++;
				workaddr = addr + (blocktbl[blockswritten] * blocksize * 2) + 9;
			}
		}

		free(blocktbl);

		sh.regs.r[0] = 0; // returns 0 if there's no error
		sh.regs.pc = sh.regs.pr;
	}

	//////////////////////////////////////////////////////////////////////////////

	void FASTCALL BiosBUPRead(Sh2 &sh)
	{
		u32 size;
		u32 addr;
		u32 blocksize;
		u32 block;
		u32 ret;
		u32 tableaddr;
		u16 *blocktbl;
		int numblocks;
		int blocksread;
		u32 datasize;

		LOG("BiosBUPRead\n", sh.regs.pr);

		ret = GetDeviceStats(sh.regs.r[4], &size, &addr, &blocksize);

		if (ret == 1)
		{
			// Error
			sh.regs.r[0] = ret;
			sh.regs.pc = sh.regs.pr;
			return;
		}

		// See if save exists
		if ((block = FindSave(sh.regs.r[4], sh.regs.r[5], 2, size, addr, blocksize)) == 0)
		{
			// save doesn't exist
			sh.regs.r[0] = 5;
			sh.regs.pc = sh.regs.pr;
			return;
		}

		tableaddr = addr + (block * blocksize * 2) + 0x3D;
		datasize = (MappedMemoryReadByte(tableaddr) << 24) | (MappedMemoryReadByte(tableaddr + 2) << 16) |
			(MappedMemoryReadByte(tableaddr+4) << 8) | MappedMemoryReadByte(tableaddr + 6);

		// Read in Block Table
		if ((blocktbl = ReadBlockTable(addr, &tableaddr, block, blocksize, &numblocks, &blocksread)) == NULL)
		{
			// Just return an error that might make sense
			sh.regs.r[0] = 8;
			sh.regs.pc = sh.regs.pr;
			return;
		}

		// Now let's read in the data
		while (datasize > 0)
		{
			MappedMemoryWriteByte(sh.regs.r[6], MappedMemoryReadByte(tableaddr));
			datasize--;
			sh.regs.r[6]++;
			tableaddr+=2;

			if (((tableaddr-1) & ((blocksize << 1) - 1)) == 0)
			{
				// Load up the next block
				tableaddr = addr + (blocktbl[blocksread] * blocksize * 2) + 9;
				blocksread++;
			}
		}

		free(blocktbl);

		sh.regs.r[0] = 0; // returns 0 if there's no error
		sh.regs.pc = sh.regs.pr;
	}

	//////////////////////////////////////////////////////////////////////////////

	void FASTCALL BiosBUPDelete(Sh2 &sh)
	{
		u32 size;
		u32 addr;
		u32 blocksize;
		u32 block;
		u32 ret;

		LOG("BiosBUPDelete. PR = %08X\n", sh.regs.pr);

		// Fill in status variables
		ret = GetDeviceStats(sh.regs.r[4], &size, &addr, &blocksize);
		if (ret == 1)
		{
			// Error
			sh.regs.r[0] = ret;
			sh.regs.pc = sh.regs.pr;
			return;
		}

		// See if save exists
		if ((block = FindSave(sh.regs.r[4], sh.regs.r[5], 2, size, addr, blocksize)) == 0)
		{
			// Since the save doesn't exist, let's bail with an error

			sh.regs.r[0] = 5;
			sh.regs.pc = sh.regs.pr;
			return;
		}

		DeleteSave(addr, block, blocksize);

		sh.regs.r[0] = 0; // returns 0 if there's no error
		sh.regs.pc = sh.regs.pr;
	}

	//////////////////////////////////////////////////////////////////////////////

	void FASTCALL BiosBUPDirectory(Sh2 &sh)
	{
		u32 size;
		u32 addr;
		u32 blocksize;
		u32 ret;
		u32 i;
		char filename[12];
		u32 blockoffset=2;

		//   int findmatch = MappedMemoryReadByte(sh.regs.r[5]);

		for (i = 0; i < 12; i++)
			filename[i] = MappedMemoryReadByte(sh.regs.r[5]+i);

		LOG("BiosBUPDirectory. arg1 = %d, arg2 = %s, arg3 = %08X, arg4 = %08X, PR = %08X\n", sh.regs.r[4], filename, sh.regs.r[6], sh.regs.r[7], sh.regs.pr);

		ret = GetDeviceStats(sh.regs.r[4], &size, &addr, &blocksize);

		if (ret == 1)
		{
			// Error
			sh.regs.r[0] = ret;
			sh.regs.pc = sh.regs.pr;
			return;
		}

		for (i = 0; i < sh.regs.r[6]; i++)
		{
			u32 i4;
			u32 datasize=0;
			u32 block = FindSave(sh.regs.r[4], sh.regs.r[5], blockoffset, size, addr, blocksize);

			if (block == 0)
				break;

			blockoffset = block+1;

			// Alright, we found a match :) Time to copy over some data
			block = addr + (blocksize * block * 2);

			// Copy over filename
			for (i4 = block+0x9; i4 < ((block+0x9) + (11 * 2)); i4+=2)
			{
				MappedMemoryWriteByte(sh.regs.r[7], MappedMemoryReadByte(i4));
				sh.regs.r[7]++;
			}
			MappedMemoryWriteByte(sh.regs.r[7], 0);
			sh.regs.r[7]++;

			// Copy over comment
			for (i4 = block+0x21; i4 < ((block+0x21) + (10 * 2)); i4+=2)
			{
				MappedMemoryWriteByte(sh.regs.r[7], MappedMemoryReadByte(i4));
				sh.regs.r[7]++;
			}

			// Copy over language
			MappedMemoryWriteByte(sh.regs.r[7], MappedMemoryReadByte(block+0x1F));
			sh.regs.r[7]++;

			MappedMemoryWriteByte(sh.regs.r[7], 0);
			sh.regs.r[7]++;

			// Copy over date
			for (i4 = block+0x35; i4 < ((block+0x35) + (4 * 2)); i4+=2)
			{
				MappedMemoryWriteByte(sh.regs.r[7], MappedMemoryReadByte(i4));
				sh.regs.r[7]++;
			}

			// Copy over data size
			for (i4 = block+0x3D; i4 < ((block+0x3D) + (4 * 2)); i4+=2)
			{
				u8 data;
				datasize <<= 8;
				data = MappedMemoryReadByte(i4);
				MappedMemoryWriteByte(sh.regs.r[7], data);
				datasize |= data;
				sh.regs.r[7]++;
			}

			// Calculate block size from the data size, and then copy it over
			MappedMemoryWriteWord(sh.regs.r[7], (u16)(((datasize + 0x1D) / (blocksize - 6)) + 1));
			sh.regs.r[7] += 4;
		}

		sh.regs.r[0] = i; // returns the number of successfully read dir entries
		sh.regs.pc = sh.regs.pr;
	}

	//////////////////////////////////////////////////////////////////////////////

	void FASTCALL BiosBUPVerify(Sh2 &sh)
	{
		u32 size;
		u32 addr;
		u32 blocksize;
		u32 block;
		u32 ret;
		u32 tableaddr;
		u32 datasize;
		u16 *blocktbl;
		int numblocks;
		int blocksread;

		LOG("BiosBUPVerify. PR = %08X\n", sh.regs.pr);

		ret = GetDeviceStats(sh.regs.r[4], &size, &addr, &blocksize);

		if (ret == 1)
		{
			// Error
			sh.regs.r[0] = ret;
			sh.regs.pc = sh.regs.pr;
			return;
		}

		// See if save exists
		if ((block = FindSave(sh.regs.r[4], sh.regs.r[5], 2, size, addr, blocksize)) == 0)
		{
			// Since the save doesn't exist, let's bail with an error
			sh.regs.r[0] = 5; // Not found
			sh.regs.pc = sh.regs.pr;
			return;
		}

		tableaddr = addr + (block * blocksize * 2) + 0x3D;
		datasize = (MappedMemoryReadByte(tableaddr) << 24) | (MappedMemoryReadByte(tableaddr + 2) << 16) |
			(MappedMemoryReadByte(tableaddr+4) << 8) | MappedMemoryReadByte(tableaddr + 6);

		// Read in Block Table
		if ((blocktbl = ReadBlockTable(addr, &tableaddr, block, blocksize, &numblocks, &blocksread)) == NULL)
		{
			// Just return an error that might make sense
			sh.regs.r[0] = 8; // Broken
			sh.regs.pc = sh.regs.pr;
			return;
		}

		// Now let's read in the data, and check to see if it matches 
		while (datasize > 0)
		{
			if (MappedMemoryReadByte(sh.regs.r[6]) != MappedMemoryReadByte(tableaddr))
			{
				free(blocktbl);
				// Ok, the data doesn't match
				sh.regs.r[0] = 7; // No match
				sh.regs.pc = sh.regs.pr;
				return;
			}

			datasize--;
			sh.regs.r[6]++;
			tableaddr+=2;

			if (((tableaddr-1) & ((blocksize << 1) - 1)) == 0)
			{
				// Load up the next block
				tableaddr = addr + (blocktbl[blocksread] * blocksize * 2) + 9;
				blocksread++;
			}
		}

		free(blocktbl);

		sh.regs.r[0] = 0; // returns 0 if there's no error
		sh.regs.pc = sh.regs.pr;
	}

	//////////////////////////////////////////////////////////////////////////////

	void ConvertMonthAndDay(u32 data, u32 monthaddr, u32 dayaddr, int type)
	{
		int i;
		u16 monthtbl[11] = { 31, 31+28, 31+28+31, 31+28+31+30, 31+28+31+30+31,
			31+28+31+30+31+30, 31+28+31+30+31+30+31,
			31+28+31+30+31+30+31+31, 31+28+31+30+31+30+31+31+30,
			31+28+31+30+31+30+31+31+30+31,
			31+28+31+30+31+30+31+31+30+31+30 };

		if (data < monthtbl[0])
		{
			// Month
			MappedMemoryWriteByte(monthaddr, 1);

			// Day
			MappedMemoryWriteByte(dayaddr, (u8)(data + 1));
			return;
		}

		for (i = 1; i < 11; i++)
		{
			if (data <= monthtbl[i])
				break;
		}

		if (type == 1)
		{
			// Month
			MappedMemoryWriteByte(monthaddr, (u8)(i + 1));

			// Day
			if ((i + 1) == 2)
				MappedMemoryWriteByte(dayaddr, (u8)(data - monthtbl[(i - 1)] + 1));
			else
				MappedMemoryWriteByte(dayaddr, (u8)(data - monthtbl[(i - 1)]));
		}
		else
		{
			// Month
			MappedMemoryWriteByte(monthaddr, (u8)(i + 1));

			// Day
			MappedMemoryWriteByte(dayaddr, (u8)(data - monthtbl[(i - 1)] + 1));
		}
	}

	//////////////////////////////////////////////////////////////////////////////

	void FASTCALL BiosBUPGetDate(Sh2 &sh)
	{
		u32 date;
		u32 div;
		u32 yearoffset;
		u32 yearremainder;

		LOG("BiosBUPGetDate. PR = %08X\n", sh.regs.pr);

		date = sh.regs.r[4];

		// Time
		MappedMemoryWriteByte(sh.regs.r[5]+3, (u8)((date % 0x5A0) / 0x3C));

		// Minute
		MappedMemoryWriteByte(sh.regs.r[5]+4, (u8)(date % 0x3C));

		div = date / 0x5A0;

		// Week
		if (div > 0xAB71)
			MappedMemoryWriteByte(sh.regs.r[5]+5, (u8)((div + 1) % 7));
		else
			MappedMemoryWriteByte(sh.regs.r[5]+5, (u8)((div + 2) % 7));

		yearremainder = div % 0x5B5;

		if (yearremainder > 0x16E)
		{
			yearoffset = (yearremainder - 1) / 0x16D;
			ConvertMonthAndDay((yearremainder - 1) % 0x16D, sh.regs.r[5]+1, sh.regs.r[5]+2, 0);
		}
		else
		{
			yearoffset = 0;
			ConvertMonthAndDay(0, sh.regs.r[5]+1, sh.regs.r[5]+2, 1);
		}

		// Year
		MappedMemoryWriteByte(sh.regs.r[5], (u8)(((div / 0x5B5) * 4) + yearoffset));

		sh.regs.pc = sh.regs.pr;
	}

	//////////////////////////////////////////////////////////////////////////////

	void FASTCALL BiosBUPSetDate(Sh2 &sh)
	{
		u32 date;
		u8 data;
		u32 remainder;
		u16 monthtbl[11] = { 31, 31+28, 31+28+31, 31+28+31+30, 31+28+31+30+31,
			31+28+31+30+31+30, 31+28+31+30+31+30+31,
			31+28+31+30+31+30+31+31, 31+28+31+30+31+30+31+31+30,
			31+28+31+30+31+30+31+31+30+31,
			31+28+31+30+31+30+31+31+30+31+30 };

		LOG("BiosBUPSetDate. PR = %08X\n", sh.regs.pr);

		// Year
		data = MappedMemoryReadByte(sh.regs.r[4]);
		date = (data / 4) * 0x5B5;
		remainder = data % 4;
		if (remainder)
			date += (remainder * 0x16D) + 1;

		// Month
		data = MappedMemoryReadByte(sh.regs.r[4]+1);
		if (data != 1 && data < 13)
		{
			date += monthtbl[data - 2];
			if (date > 2 && remainder == 0)
				date++;
		}

		// Day
		date += MappedMemoryReadByte(sh.regs.r[4]+2) - 1;
		date *= 0x5A0;

		// Hour
		date += (MappedMemoryReadByte(sh.regs.r[4]+3) * 0x3C);

		// Minute
		date += MappedMemoryReadByte(sh.regs.r[4]+4);

		sh.regs.r[0] = date;
		sh.regs.pc = sh.regs.pr;
	}

	//////////////////////////////////////////////////////////////////////////////

	void FASTCALL BiosHandleScuInterrupt(Sh2 &sh, int vector)
	{
		// Save R0-R7, PR, GBR, and old Interrupt mask to stack
		sh.regs.r[15] -= 4;
		MappedMemoryWriteLong(sh.regs.r[15], sh.regs.r[0]);
		sh.regs.r[15] -= 4;
		MappedMemoryWriteLong(sh.regs.r[15], sh.regs.r[1]);
		sh.regs.r[15] -= 4;
		MappedMemoryWriteLong(sh.regs.r[15], sh.regs.r[2]);
		sh.regs.r[15] -= 4;
		MappedMemoryWriteLong(sh.regs.r[15], sh.regs.r[3]);
		sh.regs.r[15] -= 4;
		MappedMemoryWriteLong(sh.regs.r[15], MappedMemoryReadLong(0x06000348));
		sh.regs.r[15] -= 4;
		MappedMemoryWriteLong(sh.regs.r[15], sh.regs.r[4]);
		sh.regs.r[15] -= 4;
		MappedMemoryWriteLong(sh.regs.r[15], sh.regs.r[5]);
		sh.regs.r[15] -= 4;
		MappedMemoryWriteLong(sh.regs.r[15], sh.regs.r[6]);
		sh.regs.r[15] -= 4;
		MappedMemoryWriteLong(sh.regs.r[15], sh.regs.r[7]);
		sh.regs.r[15] -= 4;
		MappedMemoryWriteLong(sh.regs.r[15], sh.regs.pr);
		sh.regs.r[15] -= 4;
		MappedMemoryWriteLong(sh.regs.r[15], sh.regs.gbr);

		// Set SR according to vector
		sh.regs.sr = (u32)sh2masklist[vector - 0x40];

		// Write new Interrupt mask value   
		MappedMemoryWriteLong(0x06000348, MappedMemoryReadLong(0x06000348) | scumasklist[vector - 0x40]);
		MappedMemoryWriteLong(0x25FE00A0, MappedMemoryReadLong(0x06000348) | scumasklist[vector - 0x40]);

		// Set PR to our Interrupt Return handler
		sh.regs.pr = 0x00000480;

		// Now execute the interrupt
		sh.regs.pc = MappedMemoryReadLong(0x06000900+(vector << 2));
		//   LOG("Interrupt PC = %08X. Read from %08X\n", sh.regs.pc, 0x06000900+(vector << 2));

		sh.regs.cycles += 33;
	}

	//////////////////////////////////////////////////////////////////////////////

	void FASTCALL BiosHandleScuInterruptReturn(Sh2 &sh)
	{
		u32 oldmask;

		// Restore R0-R7, PR, GBR, and old Interrupt mask from stack
		sh.regs.gbr = MappedMemoryReadLong(sh.regs.r[15]);
		sh.regs.r[15] += 4;
		sh.regs.pr = MappedMemoryReadLong(sh.regs.r[15]);
		sh.regs.r[15] += 4;
		sh.regs.r[7] = MappedMemoryReadLong(sh.regs.r[15]);
		sh.regs.r[15] += 4;
		sh.regs.r[6] = MappedMemoryReadLong(sh.regs.r[15]);
		sh.regs.r[15] += 4;
		sh.regs.r[5] = MappedMemoryReadLong(sh.regs.r[15]);
		sh.regs.r[15] += 4;
		sh.regs.r[4] = MappedMemoryReadLong(sh.regs.r[15]);
		sh.regs.r[15] += 4;
		// Return SR back to normal
		sh.regs.sr = 0xF0;
		oldmask = MappedMemoryReadLong(sh.regs.r[15]);
		MappedMemoryWriteLong(0x06000348, oldmask);
		MappedMemoryWriteLong(0x25FE00A0, oldmask);
		sh.regs.r[15] += 4;
		sh.regs.r[3] = MappedMemoryReadLong(sh.regs.r[15]);
		sh.regs.r[15] += 4;
		sh.regs.r[2] = MappedMemoryReadLong(sh.regs.r[15]);
		sh.regs.r[15] += 4;
		sh.regs.r[1] = MappedMemoryReadLong(sh.regs.r[15]);
		sh.regs.r[15] += 4;
		sh.regs.r[0] = MappedMemoryReadLong(sh.regs.r[15]);
		sh.regs.r[15] += 4;

		sh.regs.pc = MappedMemoryReadLong(sh.regs.r[15]);
		sh.regs.r[15] += 4;
		sh.regs.sr = MappedMemoryReadLong(sh.regs.r[15]) & 0x000003F3;
		sh.regs.r[15] += 4;

		sh.regs.cycles += 24;
	}

	//////////////////////////////////////////////////////////////////////////////

	int FASTCALL BiosHandleFunc(Sh2 &sh)
	{
		// Let's see if it's a bios function
		switch((sh.regs.pc - 0x200) >> 2)
		{
		case 0x04: // 0x06000210
			BiosPowerOnMemoryClear(sh);
			break;
		case 0x1B: // 0x0600026C
			BiosExecuteCDPlayer(sh);
			break;
		case 0x1D: // 0x06000274
			BiosCheckMPEGCard(sh);
			break;
		case 0x20: // 0x06000280
			BiosChangeScuInterruptPriority(sh);
			break;
		case 0x27: // 0x0600029C
			BiosCDINIT2(sh);
			break;
		case 0x37: // 0x060002DC
			BiosCDINIT1(sh);
			break;
		case 0x40: // 0x06000300
			BiosSetScuInterrupt(sh);
			break;
		case 0x41: // 0x06000304
			BiosGetScuInterrupt(sh);
			break;
		case 0x44: // 0x06000310
			BiosSetSh2Interrupt(sh);
			break;
		case 0x45: // 0x06000314
			BiosGetSh2Interrupt(sh);
			break;
		case 0x48: // 0x06000320
			BiosChangeSystemClock(sh);
			break;
		case 0x4C: // 0x06000330
			BiosGetSemaphore(sh);
			break;
		case 0x4D: // 0x06000334
			BiosClearSemaphore(sh);
			break;
		case 0x50: // 0x06000340
			BiosSetScuInterruptMask(sh);
			break;
		case 0x51: // 0x06000344
			BiosChangeScuInterruptMask(sh);
			break;
		case 0x56: // 0x06000358
			BiosBUPInit(sh);
			break;
		case 0x60: // 0x06000380
			break;
		case 0x61: // 0x06000384
			BiosBUPSelectPartition(sh);
			break;
		case 0x62: // 0x06000388
			BiosBUPFormat(sh);
			break;
		case 0x63: // 0x0600038C
			BiosBUPStatus(sh);
			break;
		case 0x64: // 0x06000390
			BiosBUPWrite(sh);
			break;
		case 0x65: // 0x06000394
			BiosBUPRead(sh);
			break;
		case 0x66: // 0x06000398
			BiosBUPDelete(sh);
			break;
		case 0x67: // 0x0600039C
			BiosBUPDirectory(sh);
			break;
		case 0x68: // 0x060003A0
			BiosBUPVerify(sh);
			break;
		case 0x69: // 0x060003A4
			BiosBUPGetDate(sh);
			break;
		case 0x6A: // 0x060003A8
			BiosBUPSetDate(sh);
			break;
		case 0x6B: // 0x060003AC
			break;
		case 0x80: // Interrupt Handler
		case 0x81:
		case 0x82:
		case 0x83:
		case 0x84:
		case 0x85:
		case 0x86:
		case 0x87:
		case 0x88:
		case 0x89:
		case 0x8A:
		case 0x8B:
		case 0x8C:
		case 0x8D:
		case 0x90:
		case 0x91:
		case 0x92:
		case 0x93:
		case 0x94:
		case 0x95:
		case 0x96:
		case 0x97:
		case 0x98:
		case 0x99:
		case 0x9A:
		case 0x9B:
		case 0x9C:
		case 0x9D:
		case 0x9E:
		case 0x9F:
			BiosHandleScuInterrupt(sh, (sh.regs.pc - 0x300) >> 2);
			break;
		case 0xA0: // Interrupt Handler Return
			BiosHandleScuInterruptReturn(sh);
			break;
		default:
			return 0;
		}

		return 1;
	}

	//////////////////////////////////////////////////////////////////////////////

	deviceinfo_struct *BupGetDeviceList(int *numdevices)
	{
		deviceinfo_struct *device;
#if 0
		int devicecount=1;

		if ((CartridgeArea->cartid & 0xF0) == 0x20)
			devicecount++;

		if ((device = (deviceinfo_struct *)malloc(devicecount * sizeof(deviceinfo_struct))) == NULL)
		{
			*numdevices = 0;
			return NULL;
		}

		*numdevices = devicecount;

		device[0].id = 0;
		sprintf(device[0].name, "Internal Backup RAM");

		if ((CartridgeArea->cartid & 0xF0) == 0x20)
		{
			device[1].id = 1;
			sprintf(device[1].name, "%d Mbit Backup RAM Cartridge", 1 << ((CartridgeArea->cartid & 0xF)+1));  
		}

		// For now it's only internal backup ram and cartridge, no floppy :(
		//   device[2].id = 2;
		//   sprintf(device[1].name, "Floppy Disk Drive");
#endif
		assert(false);
		return device;
	}

	//////////////////////////////////////////////////////////////////////////////

	int BupGetStats(u32 device, u32 *freespace, u32 *maxspace)
	{
		u32 ret;
		u32 size;
		u32 addr;
		u32 blocksize;

		ret = GetDeviceStats(device, &size, &addr, &blocksize);

		// Make sure there's a proper header, and return if there's any other errors
		if (ret == 1 || CheckHeader(device) != 0)
			return 0;

		*maxspace = size / blocksize;
		assert(false);
//		*freespace = GetFreeSpace(device, size, addr, blocksize);

		return 1;
	}

	//////////////////////////////////////////////////////////////////////////////

	saveinfo_struct *BupGetSaveList(u32 device, int *numsaves)
	{
		u32 ret;
		u32 size;
		u32 addr;
		u32 blocksize;
		saveinfo_struct *save;
		int savecount=0;
		u32 i, j;
		u32 workaddr;

		ret = GetDeviceStats(device, &size, &addr, &blocksize);

		// Make sure there's a proper header, and return if there's any other errors
		if (ret == 1 || CheckHeader(device) != 0)
		{
			*numsaves = 0;
			return NULL;
		}

		for (i = ((2 * blocksize) << 1); i < (size << 1); i += (blocksize << 1))
		{
			// Find a block with the start of a save
			if (((s8)MappedMemoryReadByte(addr + i + 1)) < 0)
				savecount++;
		}

		if ((save = (saveinfo_struct *)malloc(savecount * sizeof(saveinfo_struct))) == NULL)
		{
			*numsaves = 0;
			return NULL;
		}

		*numsaves = savecount;

		savecount = 0;

		for (i = ((2 * blocksize) << 1); i < (size << 1); i += (blocksize << 1))
		{
			// Find a block with the start of a save
			if (((s8)MappedMemoryReadByte(addr + i + 1)) < 0)
			{
				workaddr = addr + i;

				// Copy over filename
				for (j = 0; j < 11; j++)
					save[savecount].filename[j] = MappedMemoryReadByte(workaddr+0x9+(j * 2));
				save[savecount].filename[11] = '\0';

				// Copy over comment
				for (j = 0; j < 10; j++)
					save[savecount].comment[j] = MappedMemoryReadByte(workaddr+0x21+(j * 2));
				save[savecount].comment[10] = '\0';

				// Copy over language
				save[savecount].language = MappedMemoryReadByte(workaddr+0x1F);

				// Copy over Date(fix me)
				save[savecount].year = 0;
				save[savecount].month = 0;
				save[savecount].day = 0;
				save[savecount].hour = 0;
				save[savecount].minute = 0;
				save[savecount].week = 0;

				// Copy over data size
				save[savecount].datasize = (MappedMemoryReadByte(workaddr+0x3D) << 24) |
					(MappedMemoryReadByte(workaddr+0x3F) << 16) |
					(MappedMemoryReadByte(workaddr+0x41) << 8) |
					MappedMemoryReadByte(workaddr+0x43);

				// Calculate size in blocks
				save[savecount].blocksize = CalcSaveSize(workaddr+0x45, blocksize) + 1;
				savecount++;
			}
		}

		return save;
	}

	//////////////////////////////////////////////////////////////////////////////

	int BupDeleteSave(u32 device, const char *savename)
	{
		u32 ret;
		u32 size;
		u32 addr;
		u32 blocksize;
		u32 block;

		ret = GetDeviceStats(device, &size, &addr, &blocksize);

		// Make sure there's a proper header, and return if there's any other errors
		if (ret == 1 || CheckHeader(device) != 0)
			return -1;

		// Let's find and delete the save game
		if ((block = FindSave2(device, savename, 2, size, addr, blocksize)) != 0)
		{
			// Delete old save
			DeleteSave(addr, block, blocksize);
			return 0;
		}

		return -2;
	}

	//////////////////////////////////////////////////////////////////////////////

	void BupFormat(u32 device)
	{
		assert(false);
#if 0
		switch (device)
		{
		case 0:
			FormatBackupRam(BupRam, 0x10000);
			break;
		case 1:
			if ((CartridgeArea->cartid & 0xF0) == 0x20)
			{
				switch (CartridgeArea->cartid & 0xF)
				{
				case 1:
					FormatBackupRam(CartridgeArea->bupram, 0x100000);
					break;
				case 2:
					FormatBackupRam(CartridgeArea->bupram, 0x200000);
					break;
				case 3:
					FormatBackupRam(CartridgeArea->bupram, 0x400000);
					break;
				case 4:
					FormatBackupRam(CartridgeArea->bupram, 0x800000);
					break;
				default: break;
				}
			}
			break;
		case 2:
			LOG("Formatting FDD not supported\n");
		default: break;
		}
#endif
	}

	//////////////////////////////////////////////////////////////////////////////

	int BupCopySave(UNUSED u32 srcdevice, UNUSED u32 dstdevice, UNUSED const char *savename)
	{
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////////

	int BupImportSave(UNUSED u32 device, const char *filename)
	{
		FILE *fp;
		u32 filesize;
		u8 *buffer;

		if (!filename)
			return -1;

		if ((fp = fopen(filename, "rb")) == NULL)
			return -1;

		// Calculate file size
		fseek(fp, 0, SEEK_END);
		filesize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		if ((buffer = (u8 *)malloc(filesize)) == NULL)
		{
			fclose(fp);
			return -2;
		}

		fread((void *)buffer, 1, filesize, fp);
		fclose(fp);

		// Write save here

		free(buffer);
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////////

	int BupExportSave(UNUSED u32 device, UNUSED const char *savename, UNUSED const char *filename)
	{
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////////

};
#include "syscall.h"
#include "copyright.h"

void main()
{
	// Khai bao
	int f_Success; // Bien co dung de kiem tra thanh cong
	SpaceId  si_voinuoc, si_result;	// Bien id cho file
	char c_readFile;	// Bien ki tu luu ki tu doc tu file
	int v1, v2;		// Voi 1, voi 2
	int v;			// Dung tich nuoc cua sinh vien
	int flag_done_result;	// Bien co luu dau hieu doc xong file result

	//-----------------------------------------------------------
	
	PrintString("Started voinuoc successfully\n");
	v1 = v2 = 0;
	// Xu ly voi nuoc
	// WHILE(11111111111111111111111111111111111111)
	
	while(1)
	{
		Wait("m_vn");

		// Mo file result.txt de ghi voi nao su dung
		PrintString("Opening result.txt in voinuoc\n");
		si_result = Open("result.txt", 0);
		if(si_result == -1)
		{		//?
			Signal("sinhvien");
			return;
		}
		PrintString("Opened result.txt in voinuoc successfully\n");

		while(1)
		{
			Wait("voinuoc");
			c_readFile = 0;			
			// Mo file voi nuoc .txt de doc dung tich
			si_voinuoc = Open("voinuoc.txt", 1);
			if(si_voinuoc == -1)
			{
				//?
				Close(si_result);
				Signal("sinhvien");
			
				return;
			}
			PrintString("Address of si_voinuoc = ");
			PrintNum(si_voinuoc);
			PrintString("\nReading the water volume:\n");
			v = 0;
			flag_done_result = 0;
			
			while(1)
			{			
				if(Read(&c_readFile, 1, si_voinuoc)  == -3 || ((c_readFile < '0' || c_readFile >9) && c_readFile != '*'))
				{	
					Close(si_voinuoc);			
					break;
				}
				PrintString("Current char: ");
				PrintChar(c_readFile);
				PrintChar('\n');

				if(c_readFile != '*')
				{
					v = v* 10 + (c_readFile - 48);
				}

				else
				{
					flag_done_result = 1;				
					Close(si_voinuoc);
					break;			
				}
			
			}
			
			
			
			if(v!= 0)
			{
				// Dung voi 1
				if(v1 <= v2)
				{
					v1 += v;
					Write("1", 1, si_result);
				}
				else	// Dung voi 2
				{					
					v2 += v;
					Write("2", 1, si_result);
					
				}
			}
			
			if(flag_done_result == 1)
			{
				v1 = v2 = 0;
				Close(si_result);				
				Signal("sinhvien");
				break;				
			}	
			
			PrintString("\nvoinuoc signals sinhvien to get the next volume\n");
			Signal("sinhvien");
			Wait("voinuoc");
		}

	}			
}

#include "syscall.h"
#include "copyright.h"

void main()
{
	// Khai bao
	int f_Success; // Bien co dung de kiem tra thanh cong
	SpaceId si_sinhvien, si_voinuoc;	// Bien id cho file
	char c_readFile;	// Bien ki tu luu ki tu doc tu file
	int flag_VN;		// Bien co de nhay den tien trinh voinuoc
	int flag_MAIN;		// Bien co de nhay den tien trinh main
	int lengthFile;		// Luu do dai file
	int i_File;		// Luu con tro file
	//-----------------------------------------------------------
	PrintString("Started sinhvien successfully\n");
	Signal("m_vn");	

	while(1)
	{
		lengthFile = 0;
		PrintString("sinhvien is waiting\n");
		Wait("sinhvien");
		PrintString("Someone signaled sinhvien\n");

		// Tao file result.txt de ghi voi nao su dung
		PrintString("Creating result.txt\n");
		f_Success = CreateFile("result.txt");
		if(f_Success == -1)
		{
			Signal("main"); // tro ve tien trinh chinh
			return;
		}
		//PrintString("Created result.txt successfully\n");

		// Mo file sinhvien.txt len de doc
		PrintString("Open sinhvien.txt in sinhvien process\n");
		si_sinhvien = Open("sinhvien.txt", 1);
		if(si_sinhvien == -1)
		{
			Signal("main"); // tro ve tien trinh chinh
			return;
		}
		PrintString("Open sinhvien.txt successfully at address: ");
		PrintNum(si_sinhvien);
		PrintString(" (sinhvien)\n");
		
		PrintString("Seeking\n");
		lengthFile = Seek(-1, si_sinhvien);
		Seek(0, si_sinhvien);
		i_File = 0;
		PrintString("Seeked successfully\n");

		
		// Tao file voinuoc.txt
		PrintString("Creating voinuoc.txt in sinhvien\n");
		f_Success = CreateFile("voinuoc.txt");
		if(f_Success == -1)
		{
			Close(si_sinhvien);
			Signal("main"); // tro ve tien trinh chinh
			return;
		}
		//PrintString("Created voinuoc.txt successfully\n");
		

		// Mo file voinuoc.txt de ghi tung dung tich nuoc cua sinhvien
		Wait("m_vn"); //7735
		PrintString("Opening voinuoc.txt\n");
		si_voinuoc = Open("voinuoc.txt", 0);
		if(si_voinuoc == -1)
		{
			PrintNum(si_voinuoc);
			PrintString(" ==> xxxxxxxxxxxxxxxxxFAILED to open voinuoc.txtxxxxxxxxxxxxxxxxxxx\n");
			Close(si_sinhvien);
			Signal("main"); // tro ve tien trinh chinh
			return;
		}


		PrintString("Opened voinuoc.txt successfully at address: ");
		PrintNum(si_voinuoc);

		// Ghi dung tich vao file voinuoc.txt tu file sinhvien.txt
		while(i_File < lengthFile)
		{
			flag_VN = 0;
			Read(&c_readFile, 1, si_sinhvien);
			PrintString("Current char: ");
			PrintChar(c_readFile);
			PrintString(" (sinhvien)\n");
			if(c_readFile != ' ')
			{
				Write(&c_readFile, 1, si_voinuoc);
				
			}
			else
			{
				flag_VN = 1;
			}



			if(i_File == lengthFile - 1)
			{
				Write("*", 1, si_voinuoc);
				flag_VN = 1;
			}
			
			
			if(flag_VN == 1)
			{
				Close(si_voinuoc);
				PrintString("Closed voinuoc.txt successfully\n");


				PrintString("sinhvien signals voinuoc\n");
				Signal("m_vn"); //7735
				Signal("voinuoc");
				
				
				// Dung chuong trinh sinhvien lai de voinuoc thuc thi
				Wait("sinhvien");
				PrintString("sinhvien has been waken up by voinuoc\n");
				
				// Tao file voinuoc.txt
				Wait("m_vn"); //7735
				f_Success = CreateFile("voinuoc.txt");
				if(f_Success == -1)
				{
					Close(si_sinhvien);
					Signal("main"); // tro ve tien trinh chinh
					return;
				}
		

				// Mo file voinuoc.txt de ghi tung dung tich nuoc cua sinhvien
				si_voinuoc = Open("voinuoc.txt", 0);
				if(si_voinuoc == -1)
				{
					Close(si_sinhvien);
					Signal("main"); // tro ve tien trinh chinh
					return;
				}
				Signal("m_vn"); //7735
				
				
			}
			
			i_File++;							
		}
		Close(si_voinuoc);
		Close(si_sinhvien);
		PrintString("Now in sinhvien and done 1 line. Then call main to get the next line\n");	
				
		// Ket thuc tien trinh sinhvien va voinuoc quay lai ham main
		
		Signal("main");	
	}
	Exit(0);
	
}

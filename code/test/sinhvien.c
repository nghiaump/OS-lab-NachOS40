#include "syscall.h"
#include "copyright.h"

#define WATER_VOLUME 10
#define COUNT_TO_WAIT 56789

void main()
{
	int si_output, id_SV, i, j, lengthFile;
	char cID, cSpace;

	
	id_SV = GetPID();				// Su dung syscall GetPID() de lay so thu tu cho sinh vien
	cID = (char)(id_SV + '0');		// Chuyen int sang char
	cSpace = ' ';					// Dau cach

	// Vong lap lay lan luot tung lit nuoc
	for(i = 1; i <= WATER_VOLUME; ++i){
		Wait("voinuoc");

		for(j = 0; j < COUNT_TO_WAIT; ++j); // Vong lap mo phong thoi gian lay 1 lit nuoc		
		
		PrintChar(cID);
		PrintChar('\n');		

		si_output = Open("output.txt", 0);	// Mo file output.txt ghi so thu tu cua sinh vien hien tai

		lengthFile = Seek(-1, si_output);	// Su dung Seek()
		Seek(lengthFile, si_output);		// de ghi tiep gia tri vao output.txt

		Write(&cID, 1, si_output);			// Ghi id_SV
		Write(&cSpace, 1, si_output);		// Ghi khoang trang
		Close(si_output);
		Signal("voinuoc");					// Lay xong 1 lit nuoc, toi luot sinh vien khac (hoac van sinh vien nay)
	}

	Exit(0);								// Ket thuc lay nuoc cua sinh vien id_SV
}

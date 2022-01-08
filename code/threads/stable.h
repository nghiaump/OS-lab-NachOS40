#ifndef STABLE_H
#define STABLE_H
#include "bitmap.h"
#include "sem.h"
#define MAX_SEMAPHORE 10
class STable{
private:
    Bitmap* bm; // quản lý slot trống
    Sem* semTab[MAX_SEMAPHORE]; // quản lý tối đa 10 đối tượng Sem   
    
public:    
    // khởi tạo size đối tượng Sem để quản lý 10 Semaphore. Gán giá trị ban đầu là null
    // nhớ khởi tạo bm để sử dụng
    STable();
    ~STable();
    int Create(char* name, int init);
    int Wait(char* name);
    int Signal(char* name);
    int FindFreeSlot();
    // hủy các đối tượng đã tạo
    // Kiểm tra Semaphore “name” chưa tồn tại thì tạo
    // Nếu tồn tại Semaphore “name” thì gọi this->P()để
    // Nếu tồn tại Semaphore “name” thì gọi this->V()để
    // Tìm slot trống
};
#endif
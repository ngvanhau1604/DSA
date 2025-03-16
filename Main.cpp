#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <time.h>
#include <sstream>
#include <iomanip>
#include <algorithm>
using namespace std;

// Custom implementation of strptime
tm *strptime(const char *s, const char *f, tm *tm)
{
    istringstream input(s);
    input >> get_time(tm, f);
    if (input.fail())
    {
        return nullptr;
    }
    return tm;
}

// Cấu trúc dữ liệu cho Máy bay
struct MayBay
{
    char soHieuMB[15]; // primary key
    char loaiMB[40];
    int soCho;
};

struct nodeMayBay
{
    int soluongMayBay = 0;
    MayBay *nodes[300];
} dsMayBay;

// Cấu trúc dữ liệu cho Vé
struct Ve
{
    int soVe;
    char soCMND[15]; // primary key
};

struct nodeVe
{
    Ve data;
    nodeVe *next = NULL;
};

// Cấu trúc dữ liệu cho Chuyến bay
struct ChuyenBay
{
    char maCB[15]; // primary key
    tm ngayGioKhoiHanh;
    string sanBayDen;
    int trangThai; // 0: huy chuyen, 1: con ve, 2: het ve, 3: hoan tat
    char soHieuMB[15];
    nodeVe dsVe; // tuy chon, tam thoi dung danh sach lien ket don
};

struct nodeChuyenBay
{
    nodeChuyenBay *next = NULL;
    ChuyenBay data;
};

nodeChuyenBay *dsChuyenBay = NULL; // dsChyenBay la head/first cua danh sach chuyen bay

// Cấu trúc dữ liệu cho Hành khách
struct HanhKhach
{
    char soCMND[15]; // primary key
    string ho;
    string ten;
    string phai;
};

// Cây nhị phân tìm kiếm cho hành khách
map<string, HanhKhach> danhSachHanhKhach;

// Hàm thêm máy bay
void themMayBay(MayBay *mb)
{
    dsMayBay.nodes[dsMayBay.soluongMayBay++] = mb;
}

// Hàm xóa máy bay
void xoaMayBay(const char *soHieuMB)
{
    for (int i = 0; i < dsMayBay.soluongMayBay; ++i)
    {
        if (strcmp(dsMayBay.nodes[i]->soHieuMB, soHieuMB) == 0)
        {
            delete dsMayBay.nodes[i];
            for (int j = i; j < dsMayBay.soluongMayBay - 1; ++j)
            {
                dsMayBay.nodes[j] = dsMayBay.nodes[j + 1];
            }
            dsMayBay.soluongMayBay--;
            break;
        }
    }
}

// Hàm hiệu chỉnh máy bay
void hieuChinhMayBay(const char *soHieuMB, const char *loaiMB, int soCho)
{
    for (int i = 0; i < dsMayBay.soluongMayBay; ++i)
    {
        if (strcmp(dsMayBay.nodes[i]->soHieuMB, soHieuMB) == 0)
        {
            strcpy(dsMayBay.nodes[i]->loaiMB, loaiMB);
            dsMayBay.nodes[i]->soCho = soCho;
            break;
        }
    }
}

// Hàm thêm chuyến bay
void themChuyenBay(ChuyenBay cb)
{
    nodeChuyenBay *newNode = new nodeChuyenBay;
    newNode->data = cb;
    newNode->next = dsChuyenBay;
    dsChuyenBay = newNode;
}

// Hàm hiệu chỉnh ngày giờ khởi hành của chuyến bay
void hieuChinhChuyenBay(const char *maCB, const char *ngayGio)
{
    nodeChuyenBay *current = dsChuyenBay;
    while (current != NULL)
    {
        if (strcmp(current->data.maCB, maCB) == 0)
        {
            strptime(ngayGio, "%a %b %d %H:%M:%S %Y", &current->data.ngayGioKhoiHanh);
            break;
        }
        current = current->next;
    }
}

// Hàm hủy chuyến bay
void huyChuyenBay(const char *maCB)
{
    nodeChuyenBay *current = dsChuyenBay;
    while (current != NULL)
    {
        if (strcmp(current->data.maCB, maCB) == 0)
        {
            current->data.trangThai = 0;
            break;
        }
        current = current->next;
    }
}

// Hàm thêm hành khách
void themHanhKhach(HanhKhach hk)
{
    danhSachHanhKhach[hk.soCMND] = hk;
}

// Hàm đặt vé
void datVe(string maCB, Ve ve)
{
    nodeChuyenBay *current = dsChuyenBay;
    while (current != NULL)
    {
        if (strcmp(current->data.maCB, maCB.c_str()) == 0)
        {
            nodeVe *newVe = new nodeVe;
            newVe->data = ve;
            newVe->next = current->data.dsVe.next;
            current->data.dsVe.next = newVe;
            break;
        }
        current = current->next;
    }
}

// Hàm hủy vé
void huyVe(string maCB, int soVe)
{
    nodeChuyenBay *current = dsChuyenBay;
    while (current != NULL)
    {
        if (strcmp(current->data.maCB, maCB.c_str()) == 0)
        {
            nodeVe *prev = &current->data.dsVe;
            nodeVe *currVe = current->data.dsVe.next;
            while (currVe != NULL)
            {
                if (currVe->data.soVe == soVe)
                {
                    prev->next = currVe->next;
                    delete currVe;
                    break;
                }
                prev = currVe;
                currVe = currVe->next;
            }
            break;
        }
        current = current->next;
    }
}

// Hàm in danh sách hành khách thuộc chuyến bay
void inDanhSachHanhKhach(string maCB)
{
    nodeChuyenBay *current = dsChuyenBay;
    while (current != NULL)
    {
        if (strcmp(current->data.maCB, maCB.c_str()) == 0)
        {
            cout << "=============================================" << endl;
            cout << "DANH SACH HANH KHACH THUOC CHUYEN BAY " << maCB << endl;
            cout << "Ngay gio khoi hanh: " << asctime(&current->data.ngayGioKhoiHanh);
            cout << "Noi den: " << current->data.sanBayDen << endl;
            cout << "---------------------------------------------" << endl;
            cout << setw(5) << "STT" << setw(10) << "SO VE" << setw(15) << "SO CMND" << setw(20) << "HO TEN" << setw(10) << "PHAI" << endl;
            cout << "---------------------------------------------" << endl;
            int stt = 1;
            nodeVe *currVe = current->data.dsVe.next;
            while (currVe != NULL)
            {
                HanhKhach hk = danhSachHanhKhach[currVe->data.soCMND];
                cout << setw(5) << stt++ << setw(10) << currVe->data.soVe << setw(15) << currVe->data.soCMND << setw(20) << hk.ho + " " + hk.ten << setw(10) << hk.phai << endl;
                currVe = currVe->next;
            }
            cout << "=============================================" << endl;
            break;
        }
        current = current->next;
    }
}

// Hàm in danh sách các chuyến bay khởi hành trong ngày đến nơi XXXX mà còn vé
void inDanhSachChuyenBayTrongNgay(string ngay, string noiDen)
{
    nodeChuyenBay *current = dsChuyenBay;
    while (current != NULL)
    {
        char buffer[80];
        strftime(buffer, 80, "%d/%m/%Y", &current->data.ngayGioKhoiHanh);
        if (strcmp(buffer, ngay.c_str()) == 0 && current->data.sanBayDen == noiDen && current->data.trangThai == 1)
        {
            cout << "Ma chuyen bay: " << current->data.maCB << endl;
            cout << "Gio khoi hanh: " << asctime(&current->data.ngayGioKhoiHanh);
            cout << "So ve con trong: " << current->data.dsVe.next << endl;
        }
        current = current->next;
    }
}

// Hàm in danh sách các vé còn trống của 1 chuyến bay có mã chuyến bay là X
void inDanhSachVeTrong(string maCB)
{
    nodeChuyenBay *current = dsChuyenBay;
    while (current != NULL)
    {
        if (strcmp(current->data.maCB, maCB.c_str()) == 0)
        {
            cout << "Danh sach ve con trong cua chuyen bay " << maCB << ":" << endl;
            nodeVe *currVe = current->data.dsVe.next;
            while (currVe != NULL)
            {
                cout << "So ve: " << currVe->data.soVe << endl;
                currVe = currVe->next;
            }
            break;
        }
        current = current->next;
    }
}

// Hàm thống kê số lượt thực hiện chuyến bay của từng máy bay
void thongKeSoLuotThucHienChuyenBay()
{
    map<string, int> thongKe;
    nodeChuyenBay *current = dsChuyenBay;
    while (current != NULL)
    {
        thongKe[current->data.soHieuMB]++;
        current = current->next;
    }

    vector<pair<string, int>> thongKeVec(thongKe.begin(), thongKe.end());
    sort(thongKeVec.begin(), thongKeVec.end(), [](const pair<string, int> &a, const pair<string, int> &b)
         { return b.second < a.second; });

    cout << "So luot thuc hien chuyen bay cua tung may bay:" << endl;
    for (const auto &entry : thongKeVec)
    {
        cout << "So hieu may bay: " << entry.first << " - So luot thuc hien: " << entry.second << endl;
    }
}

// Hàm lưu dữ liệu vào tệp
void luuDuLieu()
{
    ofstream file("data.txt");
    if (file.is_open())
    {
        // Lưu danh sách máy bay
        file << dsMayBay.soluongMayBay << endl;
        for (int i = 0; i < dsMayBay.soluongMayBay; ++i)
        {
            file << dsMayBay.nodes[i]->soHieuMB << " " << dsMayBay.nodes[i]->loaiMB << " " << dsMayBay.nodes[i]->soCho << endl;
        }

        // Lưu danh sách chuyến bay
        nodeChuyenBay *current = dsChuyenBay;
        int soLuongChuyenBay = 0;
        while (current != NULL)
        {
            soLuongChuyenBay++;
            current = current->next;
        }
        file << soLuongChuyenBay << endl;
        current = dsChuyenBay;
        while (current != NULL)
        {
            file << current->data.maCB << " " << asctime(&current->data.ngayGioKhoiHanh) << " " << current->data.sanBayDen << " " << current->data.trangThai << " " << current->data.soHieuMB << endl;
            nodeVe *currVe = current->data.dsVe.next;
            int soLuongVe = 0;
            while (currVe != NULL)
            {
                soLuongVe++;
                currVe = currVe->next;
            }
            file << soLuongVe << endl;
            currVe = current->data.dsVe.next;
            while (currVe != NULL)
            {
                file << currVe->data.soVe << " " << currVe->data.soCMND << endl;
                currVe = currVe->next;
            }
            current = current->next;
        }

        // Lưu danh sách hành khách
        file << danhSachHanhKhach.size() << endl;
        for (const auto &hk : danhSachHanhKhach)
        {
            file << hk.second.soCMND << " " << hk.second.ho << " " << hk.second.ten << " " << hk.second.phai << endl;
        }

        file.close();
    }
}

// Hàm đọc dữ liệu từ tệp
void docDuLieu()
{
    ifstream file("data.txt");
    if (file.is_open())
    {
        // Đọc danh sách máy bay
        int soLuongMayBay;
        file >> soLuongMayBay;
        for (int i = 0; i < soLuongMayBay; ++i)
        {
            MayBay *mb = new MayBay;
            file >> mb->soHieuMB >> mb->loaiMB >> mb->soCho;
            dsMayBay.nodes[dsMayBay.soluongMayBay++] = mb;
        }

        // Đọc danh sách chuyến bay
        int soLuongChuyenBay;
        file >> soLuongChuyenBay;
        for (int i = 0; i < soLuongChuyenBay; ++i)
        {
            ChuyenBay cb;
            string ngayGio;
            file >> cb.maCB >> ngayGio >> cb.sanBayDen >> cb.trangThai >> cb.soHieuMB;
            // Custom implementation of strptime
            istringstream ss(ngayGio);
            ss >> get_time(&cb.ngayGioKhoiHanh, "%a %b %d %H:%M:%S %Y");
            strptime(ngayGio.c_str(), "%a %b %d %H:%M:%S %Y", &cb.ngayGioKhoiHanh);
            int soLuongVe;
            file >> soLuongVe;
            for (int j = 0; j < soLuongVe; ++j)
            {
                Ve ve;
                file >> ve.soVe >> ve.soCMND;
                nodeVe *newVe = new nodeVe;
                newVe->data = ve;
                newVe->next = cb.dsVe.next;
                cb.dsVe.next = newVe;
            }
            nodeChuyenBay *newNode = new nodeChuyenBay;
            newNode->data = cb;
            newNode->next = dsChuyenBay;
            dsChuyenBay = newNode;
        }

        // Đọc danh sách hành khách
        int soLuongHanhKhach;
        file >> soLuongHanhKhach;
        for (int i = 0; i < soLuongHanhKhach; ++i)
        {
            HanhKhach hk;
            file >> hk.soCMND >> hk.ho >> hk.ten >> hk.phai;
            danhSachHanhKhach[hk.soCMND] = hk;
        }

        file.close();
    }
}

// Hàm hiển thị menu
void hienThiMenu()
{
    cout << "=============================================" << endl;
    cout << "QUAN LY CHUYEN BAY NOI DIA" << endl;
    cout << "1. Cap nhat danh sach may bay" << endl;
    cout << "2. Cap nhat chuyen bay" << endl;
    cout << "3. Dat ve" << endl;
    cout << "4. Huy ve" << endl;
    cout << "5. In danh sach hanh khach thuoc chuyen bay" << endl;
    cout << "6. In danh sach chuyen bay khoi hanh trong ngay" << endl;
    cout << "7. In danh sach ve con trong cua chuyen bay" << endl;
    cout << "8. Thong ke so luot thuc hien chuyen bay cua tung may bay" << endl;
    cout << "9. Luu du lieu" << endl;
    cout << "10. Doc du lieu" << endl;
    cout << "0. Thoat" << endl;
    cout << "=============================================" << endl;
    cout << "Nhap lua chon cua ban: ";
}

// Hàm xử lý lựa chọn của người dùng
void xuLyLuaChon(int luaChon)
{
    switch (luaChon)
    {
    case 1:
    {
        int subChoice;
        cout << "1. Them may bay" << endl;
        cout << "2. Xoa may bay" << endl;
        cout << "3. Hieu chinh may bay" << endl;
        cout << "Nhap lua chon cua ban: ";
        cin >> subChoice;
        if (subChoice == 1)
        {
            MayBay *mb = new MayBay;
            cout << "Nhap so hieu may bay: ";
            cin >> mb->soHieuMB;
            cout << "Nhap loai may bay: ";
            cin >> mb->loaiMB;
            cout << "Nhap so cho: ";
            cin >> mb->soCho;
            themMayBay(mb);
        }
        else if (subChoice == 2)
        {
            char soHieuMB[15];
            cout << "Nhap so hieu may bay can xoa: ";
            cin >> soHieuMB;
            xoaMayBay(soHieuMB);
        }
        else if (subChoice == 3)
        {
            char soHieuMB[15];
            char loaiMB[40];
            int soCho;
            cout << "Nhap so hieu may bay can hieu chinh: ";
            cin >> soHieuMB;
            cout << "Nhap loai may bay moi: ";
            cin >> loaiMB;
            cout << "Nhap so cho moi: ";
            cin >> soCho;
            hieuChinhMayBay(soHieuMB, loaiMB, soCho);
        }
        break;
    }
    case 2:
    {
        int subChoice;
        cout << "1. Them chuyen bay" << endl;
        cout << "2. Hieu chinh ngay gio khoi hanh" << endl;
        cout << "3. Huy chuyen bay" << endl;
        cout << "Nhap lua chon cua ban: ";
        cin >> subChoice;
        if (subChoice == 1)
        {
            ChuyenBay cb;
            cout << "Nhap ma chuyen bay: ";
            cin >> cb.maCB;
            cout << "Nhap ngay gio khoi hanh (vd: Mon Mar 15 14:30:00 2021): ";
            string ngayGio;
            cin.ignore();
            getline(cin, ngayGio);
            strptime(ngayGio.c_str(), "%a %b %d %H:%M:%S %Y", &cb.ngayGioKhoiHanh);
            cout << "Nhap san bay den: ";
            cin >> cb.sanBayDen;
            cout << "Nhap trang thai (0: huy chuyen, 1: con ve, 2: het ve, 3: hoan tat): ";
            cin >> cb.trangThai;
            cout << "Nhap so hieu may bay: ";
            cin >> cb.soHieuMB;
            themChuyenBay(cb);
        }
        else if (subChoice == 2)
        {
            char maCB[15];
            cout << "Nhap ma chuyen bay can hieu chinh: ";
            cin >> maCB;
            cout << "Nhap ngay gio khoi hanh moi (vd: Mon Mar 15 14:30:00 2021): ";
            string ngayGio;
            cin.ignore();
            getline(cin, ngayGio);
            hieuChinhChuyenBay(maCB, ngayGio.c_str());
        }
        else if (subChoice == 3)
        {
            char maCB[15];
            cout << "Nhap ma chuyen bay can huy: ";
            cin >> maCB;
            huyChuyenBay(maCB);
        }
        break;
    }
    case 3:
    {
        string maCB;
        Ve ve;
        cout << "Nhap ma chuyen bay: ";
        cin >> maCB;
        cout << "Nhap so ve: ";
        cin >> ve.soVe;
        cout << "Nhap so CMND: ";
        cin >> ve.soCMND;
        datVe(maCB, ve);
        break;
    }
    case 4:
    {
        string maCB;
        int soVe;
        cout << "Nhap ma chuyen bay: ";
        cin >> maCB;
        cout << "Nhap so ve can huy: ";
        cin >> soVe;
        huyVe(maCB, soVe);
        break;
    }
    case 5:
    {
        string maCB;
        cout << "Nhap ma chuyen bay: ";
        cin >> maCB;
        inDanhSachHanhKhach(maCB);
        break;
    }
    case 6:
    {
        string ngay, noiDen;
        cout << "Nhap ngay (vd: 15/03/2021): ";
        cin >> ngay;
        cout << "Nhap noi den: ";
        cin >> noiDen;
        inDanhSachChuyenBayTrongNgay(ngay, noiDen);
        break;
    }
    case 7:
    {
        string maCB;
        cout << "Nhap ma chuyen bay: ";
        cin >> maCB;
        inDanhSachVeTrong(maCB);
        break;
    }
    case 8:
    {
        thongKeSoLuotThucHienChuyenBay();
        break;
    }
    case 9:
    {
        luuDuLieu();
        break;
    }
    case 10:
    {
        docDuLieu();
        break;
    }
    case 0:
    {
        cout << "Thoat chuong trinh." << endl;
        exit(0);
    }
    default:
        cout << "Lua chon khong hop le. Vui long chon lai." << endl;
        break;
    }
}

int main()
{
    docDuLieu();
    int luaChon;
    do
    {
        hienThiMenu();
        cin >> luaChon;
        xuLyLuaChon(luaChon);
    } while (luaChon != 0);
    return 0;
}
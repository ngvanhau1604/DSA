#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <sstream>
#include <algorithm>

using namespace std;

struct tgian
{
    int phut;
    int gio;
    int ngay;
    int thang;
    int nam;
};

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
    tgian ngayGioKhoiHanh;
    string sanBayDen;
    int trangThai; // 0: huy chuyen, 1: con ve, 2: het ve, 3: hoan tat
    char soHieuMB[15];
    nodeVe *dsVe; // danh sách liên kết đơn
};

struct nodeChuyenBay
{
    nodeChuyenBay *next = NULL;
    ChuyenBay data;
};

nodeChuyenBay *dsChuyenBay = NULL; // dsChuyenBay là head/first của danh sách chuyến bay

// Cấu trúc dữ liệu cho Hành khách
struct HanhKhach
{
    char soCMND[15]; // primary key
    string ho;
    string ten;
    string phai;
};

// Cây nhị phân tìm kiếm cho hành khách
struct AVLNode
{
    HanhKhach data;
    AVLNode *left = nullptr;
    AVLNode *right = nullptr;
    int height = 1;
};

AVLNode *rootHanhKhach = nullptr;

// Utility function to get the height of a node
int getHeight(AVLNode *node)
{
    return node ? node->height : 0;
}

// Utility function to get the balance factor of a node
int getBalanceFactor(AVLNode *node)
{
    return node ? getHeight(node->left) - getHeight(node->right) : 0;
}

// Right rotate utility
AVLNode *rightRotate(AVLNode *y)
{
    AVLNode *x = y->left;
    AVLNode *T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

    return x;
}

// Left rotate utility
AVLNode *leftRotate(AVLNode *x)
{
    AVLNode *y = x->right;
    AVLNode *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;

    return y;
}

// Insert a new HanhKhach into the AVL tree
AVLNode *insertHanhKhach(AVLNode *node, HanhKhach hk)
{
    if (!node)
    {
        AVLNode *newNode = new AVLNode;
        newNode->data = hk;
        return newNode;
    }

    if (strcmp(hk.soCMND, node->data.soCMND) < 0)
        node->left = insertHanhKhach(node->left, hk);
    else if (strcmp(hk.soCMND, node->data.soCMND) > 0)
        node->right = insertHanhKhach(node->right, hk);
    else
        return node; // Duplicate keys are not allowed

    node->height = 1 + max(getHeight(node->left), getHeight(node->right));

    int balance = getBalanceFactor(node);

    if (balance > 1 && strcmp(hk.soCMND, node->left->data.soCMND) < 0)
        return rightRotate(node);

    if (balance < -1 && strcmp(hk.soCMND, node->right->data.soCMND) > 0)
        return leftRotate(node);

    if (balance > 1 && strcmp(hk.soCMND, node->left->data.soCMND) > 0)
    {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    if (balance < -1 && strcmp(hk.soCMND, node->right->data.soCMND) < 0)
    {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

// Search for a HanhKhach by soCMND
HanhKhach *searchHanhKhach(AVLNode *node, const char *soCMND)
{
    if (!node)
        return nullptr;

    if (strcmp(soCMND, node->data.soCMND) == 0)
        return &node->data;

    if (strcmp(soCMND, node->data.soCMND) < 0)
        return searchHanhKhach(node->left, soCMND);

    return searchHanhKhach(node->right, soCMND);
}

// Wrapper functions for adding and searching HanhKhach
void themHanhKhach(HanhKhach hk)
{
    rootHanhKhach = insertHanhKhach(rootHanhKhach, hk);
}

HanhKhach *timHanhKhach(const char *soCMND)
{
    return searchHanhKhach(rootHanhKhach, soCMND);
}

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
    nodeChuyenBay *current = dsChuyenBay;
    while (current != NULL)
    {
        if (strcmp(current->data.maCB, cb.maCB) == 0)
        {
            cout << "Da ton tai chuyen bay voi ma CB nay!" << endl;
            return;
        }
        current = current->next;
    }
    newNode->next = dsChuyenBay;
}

// Hàm hiệu chỉnh ngày giờ khởi hành của chuyến bay
void hieuChinhChuyenBay(const char *maCB, const tgian &ngayGio)
{
    nodeChuyenBay *current = dsChuyenBay;
    while (current != NULL)
    {
        if (strcmp(current->data.maCB, maCB) == 0)
        {
            current->data.ngayGioKhoiHanh = ngayGio;
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
            newVe->next = current->data.dsVe;
            current->data.dsVe = newVe;
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
            nodeVe *prev = current->data.dsVe;
            nodeVe *currVe = current->data.dsVe->next;
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
            cout << "Ngay gio khoi hanh: " << current->data.ngayGioKhoiHanh.gio << ":" << current->data.ngayGioKhoiHanh.phut << " " << current->data.ngayGioKhoiHanh.ngay << "/" << current->data.ngayGioKhoiHanh.thang + 1 << "/" << current->data.ngayGioKhoiHanh.nam << endl;
            cout << "So hieu may bay: " << current->data.soHieuMB << endl;
            cout << "Noi den: " << current->data.sanBayDen << endl;
            cout << "---------------------------------------------" << endl;
            cout << setw(5) << "STT" << setw(10) << "SO VE" << setw(15) << "SO CMND" << setw(20) << "HO TEN" << setw(10) << "PHAI" << endl;
            cout << "---------------------------------------------" << endl;
            int stt = 1;
            nodeVe *currVe = current->data.dsVe;
            while (currVe != NULL)
            {
                HanhKhach hk = *timHanhKhach(currVe->data.soCMND);
                cout << setw(5) << stt++ << setw(10) << currVe->data.soVe << setw(15) << currVe->data.soCMND << setw(20) << hk.ho + " " + hk.ten << setw(10) << hk.phai << endl;
                currVe = currVe->next;
            }
            cout << "=============================================" << endl;
            break;
        }
        current = current->next;
    }
}

// Hàm in danh sách hành khách trong cây nhị phân tìm kiếm ra file
void inDanhSachHanhKhachAVLtoFile(AVLNode *node)
{
    if (node == NULL)
        return;

    inDanhSachHanhKhachAVLtoFile(node->left);
    ofstream file("data.txt", ios::app);
    if (file.is_open())
    {
        file << node->data.soCMND << " " << node->data.ho << " " << node->data.ten << " " << node->data.phai << endl;
        file.close();
    }
    inDanhSachHanhKhachAVLtoFile(node->right);
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
            file << current->data.maCB << " "
                 << current->data.ngayGioKhoiHanh.ngay << " " << current->data.ngayGioKhoiHanh.thang << " " << current->data.ngayGioKhoiHanh.nam << " "
                 << current->data.ngayGioKhoiHanh.gio << " " << current->data.ngayGioKhoiHanh.phut << " "
                 << current->data.sanBayDen << " "
                 << current->data.trangThai << " "
                 << current->data.soHieuMB << endl;
            int soLuongVe = 0;
            nodeVe *currVe = current->data.dsVe;
            while (currVe != NULL)
            {
                soLuongVe++;
                currVe = currVe->next;
            }
            file << soLuongVe << endl;
            currVe = current->data.dsVe;
            while (currVe != NULL)
            {
                file << currVe->data.soVe << " " << currVe->data.soCMND << endl;
                currVe = currVe->next;
            }
            current = current->next;
        }

        // Lưu danh sách hành khách
        file << rootHanhKhach->height << endl;
        inDanhSachHanhKhachAVLtoFile(rootHanhKhach);

        file << endl;
        file.close();
    }
}
void kiemTra()
{
    cout << "Danh sach may bay:" << endl;
    for (int i = 0; i < dsMayBay.soluongMayBay; ++i)
    {
        cout << "So hieu: " << dsMayBay.nodes[i]->soHieuMB
             << ", Loai: " << dsMayBay.nodes[i]->loaiMB
             << ", So cho: " << dsMayBay.nodes[i]->soCho << endl;
    }

    nodeChuyenBay *current = dsChuyenBay;
    cout << "Danh sach chuyen bay:" << endl;
    while (current != NULL)
    {
        cout << "Ma CB: " << current->data.maCB
             << ", Ngay gio: " << current->data.ngayGioKhoiHanh.ngay << "/" << current->data.ngayGioKhoiHanh.thang << "/" << current->data.ngayGioKhoiHanh.nam
             << " " << current->data.ngayGioKhoiHanh.gio << ":" << current->data.ngayGioKhoiHanh.phut
             << ", Noi den: " << current->data.sanBayDen
             << ", Trang thai: " << current->data.trangThai
             << ", So hieu MB: " << current->data.soHieuMB << endl;
        current = current->next;
    }
}

// Hàm đọc dữ liệu từ tệp
void docDuLieu()
{
    ifstream file("d:\\Code\\C++\\DSA\\BigAssignment\\output\\data.txt");
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
        cout << "So luong chuyen bay: " << soLuongChuyenBay << endl;
        dsChuyenBay = NULL;
        for (int i = 0; i < soLuongChuyenBay; ++i)
        {
            nodeChuyenBay *newNode = new nodeChuyenBay;
            file >> newNode->data.maCB >> newNode->data.ngayGioKhoiHanh.ngay >> newNode->data.ngayGioKhoiHanh.thang >> newNode->data.ngayGioKhoiHanh.nam >> newNode->data.ngayGioKhoiHanh.gio >> newNode->data.ngayGioKhoiHanh.phut >> newNode->data.sanBayDen >> newNode->data.trangThai >> newNode->data.soHieuMB;

            int soLuongVe;
            file >> soLuongVe;
            newNode->data.dsVe = NULL;
            for (int j = 0; j < soLuongVe; ++j)
            {
                nodeVe *newVe = new nodeVe;
                file >> newVe->data.soVe >> newVe->data.soCMND;
                newVe->next = newNode->data.dsVe;
                newNode->data.dsVe = newVe;
            }

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
            themHanhKhach(hk);
        }
        file.close();
    }
}

void inDanhSachHanhKhachAVL(AVLNode *node)
{
    if (node == NULL)
        return;

    inDanhSachHanhKhachAVL(node->left);
    cout << "So CMND: " << node->data.soCMND
         << ", Ho ten: " << node->data.ho << " " << node->data.ten
         << ", Phai: " << node->data.phai << endl;
    inDanhSachHanhKhachAVL(node->right);
}

void thongKeSoLuotThucHienChuyenBay()
{
    map<string, int> thongKe;
    nodeChuyenBay *current = dsChuyenBay;
    while (current != NULL)
    {
        thongKe[current->data.soHieuMB]++;
        current = current->next;
    }

    cout << "Thong ke so luot thuc hien chuyen bay cua tung may bay:" << endl;
    for (const auto &entry : thongKe)
    {
        cout << "So hieu MB: " << entry.first << ", So luot thuc hien: " << entry.second << endl;
    }
}

void inDanhSachVeTrong(string maCB)
{
    nodeChuyenBay *current = dsChuyenBay;
    while (current != NULL)
    {
        if (strcmp(current->data.maCB, maCB.c_str()) == 0)
        {
            cout << "Danh sach ve con trong cua chuyen bay " << maCB << ":" << endl;
            nodeVe *currVe = current->data.dsVe;
            while (currVe != NULL)
            {
                if (strlen(currVe->data.soCMND) == 0) // Vé chưa được đặt
                {
                    cout << "So ve: " << currVe->data.soVe << endl;
                }
                currVe = currVe->next;
            }
            return;
        }
        current = current->next;
    }
    cout << "Khong tim thay chuyen bay voi ma CB: " << maCB << endl;
}

void inDanhSachChuyenBayTrongNgay(string ngay, string noiDen)
{
    cout << "Danh sach chuyen bay trong ngay " << ngay << " den noi " << noiDen << " ma con ve:" << endl;
    nodeChuyenBay *current = dsChuyenBay;
    while (current != NULL)
    {

        if (current->data.sanBayDen == noiDen && current->data.trangThai == 1)
        {
            cout << "Ma CB: " << current->data.maCB
                 << ", Gio khoi hanh: " << current->data.ngayGioKhoiHanh.gio << ":" << current->data.ngayGioKhoiHanh.phut
                 << ", Ngay khoi hanh: " << current->data.ngayGioKhoiHanh.ngay << "/" << current->data.ngayGioKhoiHanh.thang + 1 << "/" << current->data.ngayGioKhoiHanh.nam + 1900
                 << ", So hieu MB: " << current->data.soHieuMB << endl;
        }
        current = current->next;
    }
}

// Hàm hiển thị menu
void hienThiMenu()
{
    system("cls");
    cout << "=============================================" << endl;
    cout << "QUAN LY CHUYEN BAY NOI DIA" << endl;
    // a
    cout << "1. Them may bay" << endl;
    cout << "2. Xoa may bay" << endl;
    cout << "3. Hieu chinh may bay" << endl;
    // b
    cout << "4. Them chuyen bay" << endl;
    cout << "5. Hieu chinh ngay gio khoi hanh cua chuyen bay" << endl;
    cout << "6. Huy chuyen bay" << endl;
    // c
    cout << "7. Dat ve" << endl;
    // d
    cout << "8. Huy ve" << endl;
    // e
    cout << "9. In danh sach hanh khach thuoc chuyen bay" << endl;
    // f
    cout << "10. In danh sach may bay trong ngay A den noi X ma con ve" << endl;
    // g
    cout << "11. In danh sach ve con trong cua chuyen bay X" << endl;
    // h
    cout << "12. Thong ke so chuyen bay cua tung may bay" << endl;

    // additional features
    cout << "13. In danh sach may bay" << endl;
    cout << "14. In danh sach chuyen bay" << endl;
    cout << "15. In danh sach hanh khach" << endl;
    // cout << "16. Luu du lieu" << endl;
    // cout << "17. Doc du lieu" << endl;

    cout << "0. Thoat" << endl;
    cout << "=============================================" << endl;
    cout << "Nhap lua chon cua ban: ";
}

void xuLyLuaChon(int luaChon)
{
    system("cls");
    switch (luaChon)
    {
    case 1: // Thêm máy bay
    {
        MayBay *mb = new MayBay;
        cout << "Nhap so hieu may bay: ";
        cin >> mb->soHieuMB;
        cout << "Nhap loai may bay: ";
        cin >> mb->loaiMB;
        cout << "Nhap so cho: ";
        cin >> mb->soCho;
        themMayBay(mb);
        cout << "Da them may bay thanh cong!" << endl;
        system("pause");
        break;
    }
    case 2: // Xóa máy bay
    {
        char soHieuMB[15];
        cout << "Nhap so hieu may bay can xoa: ";
        cin >> soHieuMB;
        xoaMayBay(soHieuMB);
        cout << "Da xoa may bay thanh cong!" << endl;
        system("pause");
        break;
    }
    case 3: // Hiệu chỉnh máy bay
    {
        char soHieuMB[15], loaiMB[40];
        int soCho;
        cout << "Nhap so hieu may bay can hieu chinh: ";
        cin >> soHieuMB;
        cout << "Nhap loai may bay moi: ";
        cin >> loaiMB;
        cout << "Nhap so cho moi: ";
        cin >> soCho;
        hieuChinhMayBay(soHieuMB, loaiMB, soCho);
        cout << "Da hieu chinh may bay thanh cong!" << endl;
        system("pause");
        break;
    }
    case 4: // Thêm chuyến bay
    {
        ChuyenBay cb;
        cout << "Nhap ma chuyen bay: ";
        cin >> cb.maCB;
        cout << "Nhap ngay gio khoi hanh (dd mm yyyy hh mm): ";
        cin >> cb.ngayGioKhoiHanh.ngay >> cb.ngayGioKhoiHanh.thang >> cb.ngayGioKhoiHanh.nam >> cb.ngayGioKhoiHanh.gio >> cb.ngayGioKhoiHanh.phut;
        cout << "Nhap san bay den: ";
        cin >> cb.sanBayDen;
        cout << "Nhap trang thai (0: huy, 1: con ve, 2: het ve, 3: hoan tat): ";
        cin >> cb.trangThai;
        cout << "Nhap so hieu may bay: ";
        cin >> cb.soHieuMB;
        themChuyenBay(cb);
        cout << "Da them chuyen bay thanh cong!" << endl;
        system("pause");
        break;
    }
    case 5: // Hiệu chỉnh ngày giờ khởi hành của chuyến bay
    {
        char maCB[15];
        cout << "Nhap ma chuyen bay can hieu chinh: ";
        cin >> maCB;
        cout << "Nhap ngay gio khoi hanh moi (dd mm yyyy hh mm): ";
        ChuyenBay cb;
        cin >> cb.ngayGioKhoiHanh.ngay >> cb.ngayGioKhoiHanh.thang >> cb.ngayGioKhoiHanh.nam >> cb.ngayGioKhoiHanh.gio >> cb.ngayGioKhoiHanh.phut;
        hieuChinhChuyenBay(maCB, cb.ngayGioKhoiHanh);
        cout << "Da hieu chinh ngay gio khoi hanh thanh cong!" << endl;
        system("pause");
        break;
    }
    case 6: // Hủy chuyến bay
    {
        char maCB[15];
        cout << "Nhap ma chuyen bay can huy: ";
        cin >> maCB;
        huyChuyenBay(maCB);
        cout << "Da huy chuyen bay thanh cong!" << endl;
        system("pause");
        break;
    }
    case 7: // Đặt vé
    {
        string maCB;
        Ve ve;
        cout << "Nhap ma chuyen bay: ";
        cin >> maCB;
        cout << "Nhap so ve: ";
        cin >> ve.soVe;
        cout << "Nhap so CMND cua hanh khach: ";
        cin >> ve.soCMND;
        datVe(maCB, ve);
        cout << "Da dat ve thanh cong!" << endl;
        system("pause");
        break;
    }
    case 8: // Hủy vé
    {
        string maCB;
        int soVe;
        cout << "Nhap ma chuyen bay: ";
        cin >> maCB;
        cout << "Nhap so ve can huy: ";
        cin >> soVe;
        huyVe(maCB, soVe);
        cout << "Da huy ve thanh cong!" << endl;
        system("pause");
        break;
    }
    case 9: // In danh sách hành khách thuộc chuyến bay
    {
        string maCB;
        cout << "Nhap ma chuyen bay: ";
        cin >> maCB;
        inDanhSachHanhKhach(maCB);
        system("pause");
        break;
    }
    case 10: // In danh sách chuyến bay trong ngày đến nơi X mà còn vé
    {
        string ngay, noiDen;
        cout << "Nhap ngay (dd/mm/yyyy): ";
        cin >> ngay;
        cout << "Nhap noi den: ";
        cin >> noiDen;
        inDanhSachChuyenBayTrongNgay(ngay, noiDen);
        system("pause");
        break;
    }
    case 11: // In danh sách vé còn trống của chuyến bay X
    {
        string ngay, noiDen;
        cout << "Nhap ngay (dd/mm/yyyy): ";
        cin >> ngay;
        cout << "Nhap noi den: ";
        cin >> noiDen;
        inDanhSachChuyenBayTrongNgay(ngay, noiDen);
        system("pause");
        break;
    }

    case 12: // Thống kê số chuyến bay của từng máy bay
    {
        thongKeSoLuotThucHienChuyenBay();
        system("pause");
        break;
    }
    case 13: // In danh sách máy bay
    {
        cout << "Danh sach may bay:" << endl;
        for (int i = 0; i < dsMayBay.soluongMayBay; ++i)
        {
            cout << "So hieu: " << dsMayBay.nodes[i]->soHieuMB
                 << ", Loai: " << dsMayBay.nodes[i]->loaiMB
                 << ", So cho: " << dsMayBay.nodes[i]->soCho << endl;
        }
        system("pause");
        break;
    }
    case 14: // In danh sách chuyến bay
    {
        cout << "Danh sach chuyen bay:" << endl;
        nodeChuyenBay *current = dsChuyenBay;
        while (current != NULL)
        {
            cout << "Ma CB: " << current->data.maCB
                 << ", Noi den: " << current->data.sanBayDen
                 << ", Trang thai: " << current->data.trangThai
                 << ", So hieu MB: " << current->data.soHieuMB << endl;
            current = current->next;
        }
        system("pause");
        break;
    }
    case 15: // In danh sách hành khách
    {
        cout << "Danh sach hanh khach:" << endl;
        inDanhSachHanhKhachAVL(rootHanhKhach);
        system("pause");
        break;
    }

    case 0: // Thoát chương trình
        cout << "Thoat chuong trinh. Tam biet!" << endl;
        luuDuLieu();
        exit(0);
    default:
        cout << "Lua chon khong hop le. Vui long thu lai!" << endl;
    }
}

int main()
{
    cout << "Chuong trinh quan ly chuyen bay noi dia" << endl;
    docDuLieu();
    cout << "Da doc du lieu tu tep thanh cong!" << endl;
    kiemTra();
    cout << "Danh sach chuyen bay da duoc kiem tra!" << endl;
    system("pause");
    while (true)
    {
        hienThiMenu();
        int luaChon;
        cin >> luaChon;
        xuLyLuaChon(luaChon);
    }
    return 0;
}
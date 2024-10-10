#include <stdio.h>
#define size 100
typedef struct
{
	float do_dai;
	int dau, cuoi;
	int da_dung;
} canh;

void read_file(char file_name[], canh a[][size], int *n)
{
	int i, j;
	FILE *f;
	f = fopen(file_name, "r");
	if (f == NULL)
	{
		printf("LOI!!!");
		return;
	}
	// Doc so thanh pho
	fscanf(f, "%d", n);

	for (i = 0; i < *n; i++)
	{
		for (j = 0; j < *n; j++)
		{
			fscanf(f, "%f", &a[i][j].do_dai);
			a[i][j].dau = i;
			a[i][j].cuoi = j;
			a[i][j].da_dung = 0;
		}
	}
	fclose(f);
}
void in_ma_tran(canh a[][size], int n)
{
	int i, j;
	printf("Ma tran trong so cua Do Thi \n");
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
			printf(" %c%c=%8.2f", a[i][j].dau + 65, a[i][j].cuoi + 65, a[i][j].do_dai);
		printf("\n");
	}
}

void in_PA(canh PA[], int n)
{
	int i;
	float sum = 0.0;
	printf("\n Phuong an tim dc:\n");
	for (i = 0; i < n; i++)
	{
		sum += PA[i].do_dai;
		printf("Canh %c%c= %8.2f \n", PA[i].dau + 65, PA[i].cuoi + 65, PA[i].do_dai);
	}
	printf("Tong do dai cac canh cua chu trinh = %8.2f", sum);
}
// Tim canh nho nhat trong so cac canh chua su dung

float canh_NN(canh a[][size], int n)
{
	float Cmin = 3.40282e+38; // Gan cho 1 so thuc lon nhat de so sanh
	int i, j;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			if (i != j && !a[i][j].da_dung && a[i][j].do_dai < Cmin)
				Cmin = a[i][j].do_dai;
		}
	}
	return Cmin;
}
// Can duoi
float can_duoi(canh a[][size], float TGT, int n, int i)
{ // n = so thanh so, i la bac cua can duoi trong cay
	return TGT + (n - i) * canh_NN(a, n);
}
//---------
// Ghi nhan phuong an tot nhat tam thoi
//---------
void Cap_Nhat_PA_TNTT(canh a[][size], int n, float TGT, float *GiaNNTT, canh x[], canh PA[])
{
	// a la ma tran cac canh , n la so thanh pho
	// TGT , GiaNNTT , x la phuong an tam thoi, PA la phuong an tot nhat tam thoi
	int i;
	// Mang~ x da co n-1 canh tu x[0] den x[n-2]
	x[n - 1] = a[x[n - 2].cuoi][x[0].dau]; // Noi dinh cuoi cua x[n-2] voi dinh dau cua x [0] de co chu trinh
	TGT = TGT + x[n - 1].do_dai;		   // COng them vao TGT do dai cua canh noi dinh cuoi voi dinh dinh dau
	if (*GiaNNTT > TGT)
	{
		*GiaNNTT = TGT;
		for (i = 0; i < n; i++)
		{
			PA[i] = x[i];
		}
	}
}

// Gia su ta co mot phuong an thanh phan x , voi k canh
// Neu ta di toi dinh ke_tiep ma dinh ke_tiep nay trung voi mot dinh dau cua mot canh nao do
// thi tao thanh chu trinh
int co_chu_trinh(canh x[], int k, int ke_tiep)
{
	int i = 0, co_CT = 0;
	while (i < k && !co_CT)
		if (ke_tiep == x[i].dau)
			co_CT = 1;
		else
			i++;
	return co_CT;
}

void Nhanh_Can(canh a[][size], int n, int i, int dau, float *TGT, float *CD, float *GiaNNTT, canh x[], canh PA[])
{
	// a la ma tran cac canh, n la so thanh pho, i la cap cua cac nut tren cay start i =0 , dau la thanh pho khoi dau
	// TGT la tong gia tri, CD la can duoi, GiaNNTT la gia nho nhat tam thoi
	//  x la mang cua cac canh,x luu tru cac phuong an tam thoi dang xet
	//  PA luu tru mang tot nhat tam thoi
	int j; // j la dinh ke tiep
	for (j = 0; j < n; j++)
		if (dau != j && !a[dau][j].da_dung && !co_chu_trinh(x, i, j))
		{
			// UNg voi gia tri cua J , ta co mot nut tren cay
			// Tinh thong so cua nut
			*TGT = *TGT + a[dau][j].do_dai;
			*CD = can_duoi(a, *TGT, n, i + 1);
			if (*CD < *GiaNNTT)
			{
				// Neu CD >= GiaNNTT thi cat tia -> Quan trong
				// Neu ko co dieu kien nay thi chuong trinh la VET CAN

				x[i] = a[dau][j];
				a[dau][j].da_dung = 1;
				a[j][dau].da_dung = 1;

				if (i == n - 2)
				{
					// Xet het tat ca cac dinh nhung duong di moi co n-1 canh
					Cap_Nhat_PA_TNTT(a, n, *TGT, GiaNNTT, x, PA);
				}
				else
				{
					Nhanh_Can(a, n, i + 1, j, TGT, CD, GiaNNTT, x, PA); // Xet cac con cua nut i
				}
			}
			// Quay lui de xet cac nut khac (ung voi gia tri khac cua j)
			*TGT = *TGT - a[dau][j].do_dai; // Tra la TGT cua nut cha
			a[dau][j].da_dung = 0;			// Tra lai cac canh vua xet la chua su dung = 0
			a[j][dau].da_dung = 0;
		}
}

void reset(canh a[][size], int n)
{
	int i, j;
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			a[i][j].da_dung = 0;
}

int main()
{
	canh a[size][size];
	int n;
	read_file("TSP1.txt", a, &n);
	canh PA[n];
	canh x[n];
	char tpxp, yn; // tpxp thanh pho xuat phat, ys yes no dung de dieu khien chuong trrinh
	while (1)
	{ // vong lap luon luon dung
		fflush(stdin);
		in_ma_tran(a, n);
		float TGT = 0.0, CD = 0.0, GiaNNTT = 1000000;
		printf("\n Xuat phat tu thanh pho nao? Mot trong cac thanh pho tu A den %c ", n - 1 + 65);
		scanf("%c", &tpxp);
		if (tpxp >= 97)
			tpxp -= 32; // Doi chu thuong thanh chu Hoa
		Nhanh_Can(a, n, 0, tpxp - 65, &TGT, &CD, &GiaNNTT, x, PA);
		in_PA(PA, n);
		fflush(stdin);
		printf("\n Tiep tuc Y/N ? ");
		scanf("%c", &yn);
		if (yn == 'N' || yn == 'n')
			break;
		reset(a, n);
	}
	return 0;
}

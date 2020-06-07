#include <iostream>
#include <stdio.h>
#include <time.h>
#include "omp.h"
using namespace std;


void Merge(int* a, int* b, int l, int m, int r)
{
	int h, i, j, k;
	h = l;
	i = l;
	j = m + 1;

	while ((h <= m) && (j <= r))
	{
		if (a[h] <= a[j])
		{
			b[i] = a[h];
			h++;
		}
		else
		{
			b[i] = a[j];
			j++;
		}
		i++;
	}
	if (m < h)
	{
		for (k = j; k <= r; k++)
		{
			b[i] = a[k];
			i++;
		}
	}
	else
	{
		for (k = h; k <= m; k++)
		{
			b[i] = a[k];
			i++;
		}
	}
	for (k = l; k <= r; k++)
	{
		a[k] = b[k];
	}

}

void Sort(int* a, int* b, int l, int r)
{
	int m;
	if (l < r)
	{
		m = (l + r) / 2;
		Sort(a, b, l, m);
		Sort(a, b, (m + 1), r);
		Merge(a, b, l, m, r);
	}
}

int main()
{
	int Size_M = 0, PartSize = 0, Current_Rank = 0, Proc_N = 0, Last_Packet = 0;
	double WTime_Start = 0, WTime_End = 0;
    cout << "\n\tMerge sort realization with OpenMP\n\tTimofeev E.V. 381708 - 2.\n\n"  <<  "Enter the number of threads: ";
	cin >> Proc_N;
	if (Proc_N <= 0)
	{
		cout << "\n\n\tInvalid input!\nValue of threads must be greather than zero!\n\n";
		system("pause");
		return 0;
	}
	cout << "Enter the size of unsorted massive: ";
	cin >> Size_M;
	if (Size_M <= 0 || Size_M < 2)
	{
		cout << "\n\n\tInvalid input!\nSize of unsorted massive must be greather than zero and two!\n\n";
		system("pause");
		return 0;
	}
	
	PartSize = Size_M / Proc_N;
	if (Size_M % Proc_N != 0)
	{
		Last_Packet = Size_M % Proc_N;
	}

	int* Unsorted_Massive = new int[Size_M];
	int* Sorted_Massive = new int[Size_M];
	int* Middle_Massive_Before_Final = new int[Size_M];
	

	srand(time(NULL));
	cout << "\n\n";
	cout << "\tAdditional packet = " << Last_Packet << "\n\nGenerated unsorted massive: ";
	for (int i = 0; i < Size_M; i++)
	{
		Unsorted_Massive[i] = rand() % Size_M;
		cout << Unsorted_Massive[i] << "|";
	}
	cout << "\n\n";

    omp_set_num_threads(Proc_N);
    #pragma omp parallel shared(Sorted_Massive)
    {
		int* Part_Sorted_Massive = new int[PartSize];
		int* tmp_array = new int[PartSize];
		int Adrr = 0;
		
		for (int i = omp_get_thread_num() * PartSize; i < ((omp_get_thread_num() * PartSize) + PartSize); i++)
		{
			Part_Sorted_Massive[Adrr] = Unsorted_Massive[i];
			Adrr++;
		}

		#pragma omp single
		{
			WTime_Start = omp_get_wtime();
		}
		
		Sort(Part_Sorted_Massive, tmp_array, 0, (PartSize - 1));

        #pragma omp barrier 
        #pragma omp critical
        {	
			Adrr = 0;
			cout << "From process " << omp_get_thread_num() << ":\n";
			for (int i = omp_get_thread_num() * PartSize; i < ((omp_get_thread_num() * PartSize) + PartSize); i++)
			{
				Sorted_Massive[i] = Part_Sorted_Massive[Adrr];
				cout << Sorted_Massive[i] << "|";
				Adrr++;
			}			
			cout << "\n\n";
        }
		#pragma omp barrier 
		#pragma omp master
		{			
			Sort(Sorted_Massive, Middle_Massive_Before_Final, 0, (Size_M - 1));
			if (Last_Packet > 0)
			{
				for (int i = 0; i < Last_Packet; i++)
				{					
					Sorted_Massive[i] = Unsorted_Massive[(Size_M-1) - i];
				}
				Sort(Sorted_Massive, Middle_Massive_Before_Final, 0, (Size_M - 1));
			}			
			WTime_End = omp_get_wtime();
			cout << "Sorted massive: ";
			for (int i = 0; i < Size_M; i++)
			{
				cout << Sorted_Massive[i] << "|";
			}
			cout << "\n\n";
			cout << "Total time = " << WTime_End - WTime_Start << " seconds\n\n";			
		}
    }
}


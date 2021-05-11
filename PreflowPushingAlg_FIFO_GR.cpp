#include <iostream>
#include <vector>
#include <deque>
#include <limits>
#include <time.h>
#include <fstream>

using namespace std;

const int _infinum = numeric_limits<int>::max();


void push (int num, int* e, int i, int j, int** G){
    e[j] += num;
    e[i] -= num;
    G[j][i] += num; //Вычисляем остаточную пропускную способность противоположного ребра
    G[i][j] -= num; //Вычисляем остаточную пропускную способность ребра
}


int* BFS (int* d, int n, int** G){
    d[n-1] = 0;
    deque<int> dec;
    dec.push_back(n-1);
    while (!dec.empty()){
        int i = dec.front();
        dec.pop_front();
        for (int j = 0; j < n; ++j){
            if (G[j][i] != 0){
                if (d[j] == _infinum){
                    d[j] = d[i] + 1;
                    dec.push_back(j);
                }
            }
        }
    }
    return d;
}

int* GlobalRelabeling (int n, int* h, int** G){
    int* d1 = new int[n];
    fill(d1, d1 + n, _infinum);
    int* d2 = new int[n];
    fill(d2, d2 + n, _infinum);
    d1 = BFS (d1, n, G); //Ищем расстояние от всех вершин до n при помощи обхода графа в ширину по ребрам в обратном направлении
    d2 = BFS (d2, 1, G); //Ищем расстояние от всех вершин до 0 при помощи обхода графа по его обратным ребрам в обратном направлении
    for (int i = 0; i < n; ++i){
        if (d1[i] == _infinum){
            h[i] = d2[i];
        }
        else
            h[i] = d1[i];
    }
    return h;
}

int maxFlow (int n, int** G) {
    deque<int> dec; //Очередь из вершин, в которых есть избыток

    int *h = new int[n]; //Массив высот
    int *e = new int[n]; //Массив избытков

    // Инициализация
    h = GlobalRelabeling (n, h, G);
    for (int i = 0; i < n; ++i) {
        e[i] = 0;
    }

    // Пускаем максимальный поток из истока
    for (int i = 0; i < n; ++i){
        if (G[0][i] != 0){
            e[i] += G[0][i];
            e[0] -= G[0][i];
            G[i][0] += G[0][i];
            if (i != n-1)
                dec.push_back(i);
            G[0][i] = 0;
        }
    }
    h[0] = n;

    while (!dec.empty()) { //Пока в очереди есть вершины
        int i = dec.front(); //Получаем первый элемент очереди
        dec.pop_front(); //Удаляем этот первый элемент
        int minH = _infinum;
        while (e[i] > 0) { //Пока в текущей вершине есть избыток
            for (int j = 0; j < n; ++j){
                if (G[i][j] != 0){ //Проходимся по всем ребрам
                    if (h[i]-1 == h[j]) { //Если текущая вершина на 1 выше той, в которую ведет ребро, то делаем push:
                        if (j != n-1 && j != 0) { //Если вершина, в которую проталкиваем, не является истоком/стоком и не находится в очереди, то добавляем ее туда
                            bool signDec = true;
                            for (int k = 0; k < dec.size(); ++k) {
                                if (dec[k] == j)
                                    signDec = false;
                            }
                            if (signDec)
                                dec.push_back(j);
                        }
                        if (e[i] >= G[i][j]) { //Если избыток в вершине больше, чем остаточная пропускная способность ребра
                            push(G[i][j], e, i, j, G);//то проталкиваем количество единиц, равное остаточной пропускной способности
                            G[i][j] = 0;
                        } else {//Если избыток в вершине меньше, чем остаточная пропускная способность ребра
                            push(e[i], e, i, j, G);//то проталкиваем количество единиц, равное избытку
                            break; //Переходим к следующей вершине, поскольку избыток в текущей стал равен 0
                        }

                    } else { //Среди тех вершин, которые выше, либо на том же уровне, что текущая, ищем ту, высота которой минимальна (этот показатель необходим для операции relabel)
                        if (minH > h[j]) {
                            minH = h[j];
                        }
                    }
                }
            }
            if (e[i] > 0) { //Если после обхода всех ребер в вершине остался избыток, то
                h[i] = minH + 1; // relabel
                dec.push_back(i); //Добавляем эту вершину в конец очереди
                break; //Переходим к рассмотрению новой вершины
            }
        }
    }
    return e[n-1];
}

int main() {
    srand(time(0));
    ifstream dataFile;
    dataFile.open("/home/nata/CLionProjects/PreflowPushingAlg(FIFO+GR)/data2.txt");
    if (!dataFile.is_open())
        cout << "File is't open!" << endl;
    else {
        int n, m;
        dataFile >> n;
        dataFile >> m;

        int **G = new int *[n];
        for (int i = 0; i < n; ++i)
            G[i] = new int[n];
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                G[i][j] = 0;

        for (int i = 0; i < m; ++i) {
            int v1, v2, c;
            dataFile >> v1;
            dataFile >> v2;
            dataFile >> c;
            G[v1 - 1][v2 - 1] = c;
        }

        int mF;
        clock_t start, end;
        start = clock();
        mF = maxFlow(n, G);
        end = clock();
        double t = (double) (end - start) / CLOCKS_PER_SEC;
        cout << "MAXFLOW = " << mF << endl;
        cout << "TIME: " << t << endl;
    }
    return 0;
}

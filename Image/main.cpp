#include <iostream>
#include <vector>
using namespace std;

class Image
{

private:
    vector<vector<vector<int>>> image;
    int W;
    int H;
    int D;

public:

    Image(int* raw, int W, int H, int D):
            H{H}, W{W}, D{D}, image{vector<vector<vector<int>>>
                                            (H, vector<vector<int>> (W, vector<int> (D, 0)))}
    {
        for(int i=0; i<H; ++i){
            for(int j=0; j<W; ++j){
                for (int k=0; k<D; ++k){
                    image[i][j][k] = raw[k+D*(j+i*W)];
                }
            }
        }
    }
    Image(): Image(0, 0, 0){}
    Image(int W, int H, int D){
        vector<vector<vector<int>>> image(H, vector<vector<int>> (W, vector<int> (D, 0)));
    }
    
        int height(){
        return H;
    }

    int width(){
        return W;
    }

    int depth(){
        return D;
    }

    Image(Image& other){ H = other.height(); W = other.width(); D = other.depth();
        image = other.image;
    }
    
    int at(int x, int y, int d){
            return image[y][x][d];
    }
    
        void print()
    {
        int im=0;
        for (size_t i = 0; i < H; ++i){
            for (size_t j = 0; j < W; ++j){
                for (size_t t = 0; t < D; ++t){
                    im = im + image[i][j][t];
                }

                int p;
                p = im / D;
                cout <<p<<' ';
                cout <<' ';
                int x = 0;
                while (p > 0){
                    x = x + 1;
                    p = (p/10) - 1;
                }
                for (int i = 0; i <= 2 - x; ++i){
                    cout <<' ';
                }
                im = 0;
            }
            cout << '\n';

        }
    }
    
    void crop(int x1, int y1, int x2, int y2)
    {
        int H2 = y2-y1;
        int W2 = x2-x1;
        vector < vector < vector<int > > > image2(y2-y1+1, vector<vector<int>>
                (x2-x1+1, vector<int> (D, 0)));
        
        for (size_t i = 0; i < H2 + 1; ++i){
            for (size_t j=0; j < W2 + 1; ++j){
                for (size_t t = 0; t < D; ++t){
                    image2[i][j][t] = image[y1+i][x1+j][t];
                }
            }
        }
        H = H2 + 1;
        W = W2 + 1;
        image = image2;
    }



};

int main()
{

    return 0;
}

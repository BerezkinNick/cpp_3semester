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


};

int main()
{

    return 0;
}

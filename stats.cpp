
#include "stats.h"

stats::stats(PNG &im)
{
    hist.resize(im.width(), vector<vector<int>>(im.height(), vector<int>(36)));
    sumHueX.resize(im.width(), vector<double>(im.height()));
    sumHueY.resize(im.width(), vector<double>(im.height()));
    sumSat.resize(im.width(), vector<double>(im.height()));
    sumLum.resize(im.width(), vector<double>(im.height()));
    for (unsigned int y = 0; y < im.height(); y++)
    {
        for (unsigned int x = 0; x < im.width(); x++)
        {
            HSLAPixel *pixel = im.getPixel(x, y);
            if (x == 0 && y == 0)
            {
                sumHueX[x][y] = cos(PI * pixel->h / 180.0);
                sumHueY[x][y] = sin(PI * pixel->h / 180.0);
                sumSat[x][y] = pixel->s;
                sumLum[x][y] = pixel->l;

            }
            if (x != 0 && y == 0)
            {
                sumHueX[x][y] = sumHueX[x - 1][y] + cos(PI * pixel->h / 180.0);
                sumHueY[x][y] = sumHueY[x - 1][y] + sin(PI * pixel->h / 180.0);
                sumSat[x][y] = sumSat[x - 1][y] + pixel->s;
                sumLum[x][y] = sumLum[x - 1][y] + pixel->l;
            }
            if (y != 0 && x == 0)
            {
                sumHueX[x][y] = sumHueX[x][y - 1] + cos(PI * pixel->h / 180.0);
                sumHueY[x][y] = sumHueY[x][y - 1] + sin(PI * pixel->h / 180.0);
                sumSat[x][y] = sumSat[x][y - 1] + pixel->s;
                sumLum[x][y] = sumLum[x][y - 1] + pixel->l;
            }
            if (x != 0 && y != 0)
            {
                sumHueX[x][y] = sumHueX[x - 1][y] + sumHueX[x][y - 1] - sumHueX[x - 1][y - 1] + cos(PI * pixel->h / 180.0);
                sumHueY[x][y] = sumHueY[x - 1][y] + sumHueY[x][y - 1] - sumHueY[x - 1][y - 1] + sin(PI * pixel->h / 180.0);
                sumSat[x][y] = sumSat[x - 1][y] - sumSat[x - 1][y - 1] + sumSat[x][y - 1] + pixel->s;
                sumLum[x][y] = sumLum[x - 1][y] + sumLum[x][y - 1] - sumLum[x - 1][y - 1] + pixel->l;
            }

            int bin = floor(pixel->h / 10);

            for (int k = 0; k < 36; k++)
            {
                if (x == 0 && y == 0)
                {
                    hist[x][y][k] = 0;
                }
                if (x != 0 && y == 0)
                {
                    hist[x][y][k] = hist[x - 1][y][k];
                }
                else if (y != 0 && x == 0)
                {
                    //follow the top pixel
                    hist[x][y][k] = hist[x][y - 1][k];
                }
                else if (y != 0 && x != 0)
                {
                    hist[x][y][k] = hist[x - 1][y][k] + hist[x][y - 1][k] - hist[x - 1][y - 1][k];
                }
                if (k == bin)
                {
                    hist[x][y][k]++;
                }
            }
        }
    }
}

long stats::rectArea(pair<int, int> ul, pair<int, int> lr)
{

    long recArea = (lr.first - ul.first + 1) * (lr.second - ul.second + 1);
    return recArea;
}

HSLAPixel stats::getAvg(pair<int, int> ul, pair<int, int> lr)
{
    int ul_x = ul.first;
    int ul_y = ul.second;
    int lr_x = lr.first;
    int lr_y = lr.second;
    double area = rectArea(ul, lr);
    // std::cout << "areaa " << area << endl;
    double totalHueX;
    double totalHueY;

    HSLAPixel pixel;
    if (ul_x == lr_x && ul_y == lr_y) area = 1;
    if (ul_x == 0 && ul_y == 0)
    {
        totalHueX = (sumHueX[lr_x][lr_y]);
        totalHueY = (sumHueY[lr_x][lr_y]);
        pixel.s = (sumSat[lr_x][lr_y]) / area;
        pixel.l = (sumLum[lr_x][lr_y] / area);
        //std::cout << sumLum[lr_x][lr_y] << endl;
    }
    if (ul_x > 0 && ul_y > 0)
    {   
        totalHueX = (sumHueX[lr_x][lr_y] - sumHueX[ul_x - 1][lr_y] - sumHueX[lr_x][ul_y - 1] + sumHueX[ul_x - 1][ul_y - 1]);
        totalHueY = (sumHueY[lr_x][lr_y] - sumHueY[ul_x - 1][lr_y] - sumHueY[lr_x][ul_y - 1] + sumHueY[ul_x - 1][ul_y - 1]);
        pixel.s = ((sumSat[lr_x][lr_y] - sumSat[ul_x - 1][lr_y] - sumSat[lr_x][ul_y - 1] + sumSat[ul_x - 1][ul_y - 1]) / area);
        pixel.l = ((sumLum[lr_x][lr_y] - sumLum[ul_x - 1][lr_y] - sumLum[lr_x][ul_y - 1] + sumLum[ul_x - 1][ul_y - 1]) / area);
        // std::cout <<  sumLum[lr_x][lr_y] << "and" << sumLum[ul_x][lr_y]<< "and" << sumLum[lr_x][ul_y-1] << endl;
    }
    else if(ul_x == 0 && ul_y !=0) {
        totalHueX = sumHueX[lr_x][lr_y] - sumHueX[lr_x][ul_y -1];
        totalHueY = sumHueY[lr_x][lr_y] - sumHueY[lr_x][ul_y -1];
        pixel.s = ((sumSat[lr_x][lr_y] - sumSat[lr_x][ul_y -1]) / area);
        pixel.l = ((sumLum[lr_x][lr_y] - sumLum[lr_x][ul_y -1]) / area);
    }
    else if(ul_x != 0 && ul_y ==0) {
        totalHueX = sumHueX[lr_x][lr_y] - sumHueX[ul_x -1][lr_y];
        totalHueY = sumHueY[lr_x][lr_y] - sumHueY[ul_x -1][lr_y];
        pixel.s = ((sumSat[lr_x][lr_y] - sumSat[ul_x -1][lr_y]) / area);
        pixel.l = ((sumLum[lr_x][lr_y] - sumLum[ul_x -1][lr_y]) / area);
    }
    pixel.a = 1.0;
    double avgH;
    avgH = atan2(totalHueY, totalHueX) * (180.0 / PI);
    if (avgH < 0)
    {
        avgH += 360;
    }
    avgH = fmod(avgH, 360.0);
    // std::cout << "avgHue " << pixel.h  << endl;
    pixel.h = avgH;
    return pixel;
}

// HSLAPixel stats::getAvg(pair<int, int> ul, pair<int, int> lr)
// {
//     int ul_x = ul.first;
//     int ul_y = ul.second;
//     int lr_x = lr.first;
//     int lr_y = lr.second;
//     double area = rectArea(ul, lr);
//     // std::cout << "areaa " << area << endl;
//     double avgHueX;
//     double avgHueY;

//     HSLAPixel pixel;
//     if (ul_x == lr_x && ul_y == lr_y) area = 1;
//     if (ul_x == 0 && ul_y == 0)
//     {
//         avgHueX = (sumHueX[lr_x][lr_y] / area);
//         avgHueY = (sumHueY[lr_x][lr_y] / area);
//         pixel.s = (sumSat[lr_x][lr_y]) / area;
//         pixel.l = (sumLum[lr_x][lr_y] / area);
//         //std::cout << sumLum[lr_x][lr_y] << endl;
//     }
//     if (ul_x > 0 && ul_y > 0)
//     {   
//         avgHueX = ((sumHueX[lr_x][lr_y] - sumHueX[ul_x - 1][lr_y] - sumHueX[lr_x][ul_y - 1] + sumHueX[ul_x - 1][ul_y - 1]) / area);
//         avgHueY = ((sumHueY[lr_x][lr_y] - sumHueY[ul_x - 1][lr_y] - sumHueY[lr_x][ul_y - 1] + sumHueY[ul_x - 1][ul_y - 1]) / area);
//         pixel.s = ((sumSat[lr_x][lr_y] - sumSat[ul_x - 1][lr_y] - sumSat[lr_x][ul_y - 1] + sumSat[ul_x - 1][ul_y - 1]) / area);
//         pixel.l = ((sumLum[lr_x][lr_y] - sumLum[ul_x - 1][lr_y] - sumLum[lr_x][ul_y - 1] + sumLum[ul_x - 1][ul_y - 1]) / area);
//         // std::cout <<  sumLum[lr_x][lr_y] << "and" << sumLum[ul_x][lr_y]<< "and" << sumLum[lr_x][ul_y-1] << endl;
//     }
//     else if(ul_x == 0 && ul_y !=0) {
//         avgHueX = ((sumHueX[lr_x][lr_y] - sumHueX[lr_x][ul_y -1]) / area);
//         avgHueY = ((sumHueY[lr_x][lr_y] - sumHueY[lr_x][ul_y -1]) / area);
//         pixel.s = ((sumSat[lr_x][lr_y] - sumSat[lr_x][ul_y -1]) / area);
//         pixel.l = ((sumLum[lr_x][lr_y] - sumLum[lr_x][ul_y -1]) / area);
//     }
//     else if(ul_x != 0 && ul_y ==0) {
//         avgHueX = ((sumHueX[lr_x][lr_y] - sumHueX[ul_x -1][lr_y]) / area);
//         avgHueY = ((sumHueY[lr_x][lr_y] - sumHueY[ul_x -1][lr_y]) / area);
//         pixel.s = ((sumSat[lr_x][lr_y] - sumSat[ul_x -1][lr_y]) / area);
//         pixel.l = ((sumLum[lr_x][lr_y] - sumLum[ul_x -1][lr_y]) / area);
//     }
//     pixel.a = 1.0;
//     pixel.h = (atan2(avgHueY, avgHueX) * 180 / PI);
//     if (pixel.h < 0)
//     {
//         pixel.h += 360;
//     }
//     std::cout << "avgHue " << pixel.h  << endl;
//     return pixel;
// }

vector<int> stats::buildHist(pair<int, int> ul, pair<int, int> lr)
{
    // std::cout << "start BUILD HIST~ " << endl;
    int ul_x = ul.first;
    int lr_x = lr.first;
    int ul_y = ul.second;
    int lr_y = lr.second;

    vector<int> toReturn = hist[lr_x][lr_y];

    for (int i = 0; i < 36; i++)
    {
        int bin = 0; //if ul_x and ul_y ==0
        if (ul_x == 0 && ul_y == 0)
        {
            bin = hist[lr_x][lr_y][i];
        }
        else if (ul_x != 0 && ul_y == 0)
        {
            bin = hist[lr_x][lr_y][i] - hist[ul_x - 1][lr_y][i];
        }
        else if (ul_x == 0 && ul_y != 0)
        {
            bin = hist[lr_x][lr_y][i] - hist[lr_x][ul_y - 1][i];
        }
        else if (ul_x != 0 && ul_y != 0)
        {
            bin = hist[lr_x][lr_y][i] - hist[lr_x][ul_y - 1][i] - hist[ul_x - 1][lr_y][i] + hist[ul_x - 1][ul_y - 1][i];
        }
        toReturn[i] = bin;
    }
    // std::cout << "fin BUILD HIST~ " << endl;
    return toReturn;
}

// takes a distribution and returns entropy
// partially implemented so as to avoid rounding issues.
double stats::entropy(vector<int> &distn, int area)
{
    double entropy = 0.;
    for (int i = 0; i < 36; i++)
    {
        if (distn[i] > 0)
            entropy += ((double)distn[i] / (double)area) * log2((double)distn[i] / (double)area);
    }
    return -1 * entropy;
}

double stats::entropy(pair<int, int> ul, pair<int, int> lr)
{   
    // std::cout << "start entropy~ " << endl;
    vector<int> distn = buildHist(ul, lr);
    int area = rectArea(ul, lr);
    // std::cout << "ENDED entropy~ " << endl;
    return entropy(distn, area);
}
//
//  Regression Analysis with OpenCV
//  Author : Kentaro Doba
//

//#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
//#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/highgui/highgui.hpp"

#include <vector>

#include <iostream>
#include <fstream>

#include <cstdio>
#include <cstdlib>
#include <string>

using std::vector;
using std::string;
using cv::Mat;

using std::cout;
using std::cerr;
using std::endl;

static bool verbose = false;

//  split string  //
void split(string str, string delim, vector<string> &result)
{
  result.clear();
  int cut_pos;
  while( (cut_pos = str.find_first_of(delim)) != (int)string::npos ) {
    if(cut_pos > 0) {
      result.push_back(str.substr(0, cut_pos));
    }
    str = str.substr(cut_pos + 1);
  }
  if(str.length() > 0) {
    result.push_back(str);
  }
  return;
}

void
readCsv(const char *fname,
    std::vector<double> &reference,
    std::vector< std::vector<double> > &data)
{
  std::ifstream ifs;
  ifs.open(fname);

  //  init data references and variables  //
  data.clear();
  reference.clear();

  //  no file -> abort  //
  if(!ifs.is_open()) {
    cerr << "no input file : " << fname << endl;
    return;
  }

  const int bufsize(1<<12);
  char buf[bufsize];
  for(int i=0; !ifs.eof(); i++) {
    vector<double> linedata(0);
    //  get reference value (beginning of each line)   //
    ifs.getline(buf, sizeof(buf));
    vector<string> chunks;
    string str = string(buf);
    //  skip if no data exist  //
    if(str.size() <= 0) continue;
    split(str, string(","), chunks);
    //  get group of data  //
    reference.push_back(atoi(chunks[0].c_str()));
    //  @DEBUG : show training data reference  //
    if(verbose) {
      cout << "line " << i << " : " << reference[i] << ",";
    }
    //  get data segments  //
    const int variableNum = (int)chunks.size() - 1;
    for(int j=0; j<variableNum; j++) {
      linedata.push_back(atof(chunks[j+1].c_str()));
      //  @DEBUG : show data of each column  //
      if(verbose) {
        cout << linedata[j];
        if(j<variableNum-1) cout << ",";
        else cout << endl;
      }
    }
    //  end readline -> push to Mat  //
    data.push_back(linedata);
  }
  ifs.close();
}


double calcResidual(const cv::Mat &Y, const cv::Mat &X, const cv::Mat &A)
{
  double r2(0);
  cv::Mat aprxY = X * A;
  //  cal average  //
  float sumYVal(0.f);
  for(int i=0; i<Y.rows; i++) {
    sumYVal += Y.at<float>(i,0);
  }
  float aveYVal = sumYVal / (float)Y.rows;

  //  calc residual  //
  float sumRsdAll(0.f);
  float sumRsdBase(0.f);
  for(int i=0; i<Y.rows; i++) {
    float orgYVal = Y.at<float>(i,0);
    float aprxYVal = aprxY.at<float>(i,0);
    sumRsdAll += pow(orgYVal - aveYVal, 2);
    sumRsdBase += pow(aprxYVal - aveYVal, 2);
  }
  r2 = sumRsdBase / sumRsdAll;
  return r2;
}

double calcRegression(const cv::Mat &Y, const cv::Mat &X, cv::Mat &A)
{
  double r2(0);

  //  Y = X * A  //
  //  A = (Xt * X)^-1 * Xt * Y  //
  cv::Mat Xt = X.t();
  cv::Mat invXtX = (Xt * X).inv();
  cv::Mat dupA = invXtX * Xt * Y;
  dupA.copyTo(A);

  //  r2 : determination coefficient  //
  r2 = calcResidual(Y,X,A);
  return r2;
}

const char* keys =
{
  //  { short | long | init | note }  //
  "{1|||training data file name}"
  "{v|verbose|false|verbose mode}"
};

int main( int argc, const char** argv )
{
  //  @SETTING :   //
  static char printbuf[1024];
  setvbuf(stdout,printbuf,_IOLBF,sizeof(printbuf));

  //  parse options  //
  cv::CommandLineParser parser(argc, argv, keys);
  const string fileCsv = parser.get<string>("1");
  verbose = parser.get<bool>("v");

  //  training data buffer  //
  std::vector< double > reference(0);
  std::vector< std::vector<double> > data(0);
  readCsv(fileCsv.c_str(), reference, data);
  const int datasetNum = data.size();
  const int variableNum = data[0].size();

  if(datasetNum <= 0 || variableNum <= 0) {
    fprintf(stderr, "ERROR : invalid data format.\n");
    return -1;
  }

  //  data -> matrix  //
  cv::Mat refMat(datasetNum, 1, CV_32FC1);
  cv::Mat datMat(datasetNum, variableNum+1, CV_32FC1);
  cv::Mat ansMat(variableNum+1, 1, CV_32FC1);
  for(int i=0; i<datasetNum; i++) {
    refMat.at<float>(i,0) = reference[i];
    for(int j=0; j<variableNum; j++) {
      datMat.at<float>(i,j) = data[i][j];
    }
    //  Intercept  //
    datMat.at<float>(i,variableNum) = 1.0f;
  }

  double r2 = calcRegression(refMat, datMat, ansMat);

  printf("r2        = %f\n", r2);
  printf("Intercept = %f\n", ansMat.at<float>(variableNum,0));
  printf("Coef      = {\n");
  for(int i=0; i<variableNum; i++) {
    printf("  %f\n", ansMat.at<float>(i,0));
  }
  printf("}\n");

  return 0;
}

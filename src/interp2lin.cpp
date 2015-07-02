#include <RcppEigen.h>
#include <algorithm>    // to get std::lower_bound
#include <iterator>     // to get std::iterator
#include <limits>       // to get NaN

// [[Rcpp::depends(RcppEigen)]]
// [[Rcpp::export]]

Eigen::VectorXd interp2lin( const Eigen::Map<Eigen::VectorXd> & xin, const Eigen::Map<Eigen::VectorXd> & yin, const Eigen::Map<Eigen::VectorXd> & zin, const Eigen::Map<Eigen::VectorXd> & xou, const Eigen::Map<Eigen::VectorXd> & you){ 

  // Setting up initial values

  const unsigned int nXGrid = xin.size();
  const unsigned int nYGrid = yin.size();
  const unsigned int nKnownPoints = nXGrid * nYGrid;
  const unsigned int nUnknownPoints = xou.size();
 
  Eigen::VectorXd result(nUnknownPoints);

  // Preliminary checks

  if ( nXGrid != nYGrid ){
    Rcpp::stop("Input Y-grid does not have the same number of points as Input X-grid.");
  } else if ( nKnownPoints != zin.size() ) {  
    Rcpp::stop("Input Z-grid does not have the same number of points as the product of #Input Y-grid times #Input X-grid.");
  } else if ( nUnknownPoints != you.size() ){
    Rcpp::stop("Output Y-grid does not have the same number of points as Output X-grid.");
  } else if ( xin.minCoeff() >  xou.minCoeff() ){
    Rcpp::warning("Output X-grid  is outside the lower range of the input X-grid.");
  } else if ( yin.minCoeff() >  you.minCoeff() ){
    Rcpp::warning("Output X-grid  is outside the lower range of the input X-grid.");
  } else if ( xin.maxCoeff() <  xou.maxCoeff() ){
    Rcpp::warning("Output X-grid  is outside the upper ragne of the input X-grid.");
  } else if ( yin.maxCoeff() <  you.maxCoeff() ){
    Rcpp::warning("Output X-grid  is outside the upper ragne of the input X-grid.");
  } 
  
 
  const double ymin = yin.minCoeff();
  const double xmin = xin.minCoeff();
  const double ymax = yin.maxCoeff();
  const double xmax = xin.maxCoeff();

  // The actual interpolation routine
  // Check: https://en.wikipedia.org/wiki/Bilinear_interpolation#Alternative_algorithm
  // for a quick reference

  Eigen::RowVector4d fq;
  Eigen::Vector2d xa;
  Eigen::Vector2d ya;
  Eigen::Vector4d za;
  Eigen::Matrix4d A;
  // Column 1
  A.setOnes(); 

  for (unsigned int u = 0; u !=nUnknownPoints; ++u){ 
     
    if ( (xmax  < xou(u))  || (ymax <  you(u)) || // If x/you(u) is above the upper values of xin/yin 
         (xmin  > xou(u))  || (ymin >  you(u)) ){ // If x/you(u) is below the lower values of xin/yin
      result(u) = std::numeric_limits<double>::quiet_NaN() ;
      
    } else {
      // Find the appropriate x coordinates/save them in xa (2-by-1)
      // Get iterator pointing to the first element which is not less than xou(u)
      const double* x1 = std::lower_bound(&xin[0], &xin[nXGrid], xou(u));
      const double* y1 = std::lower_bound(&yin[0], &yin[nYGrid], you(u));

      xa(1) = *x1;
      ya(1) = *y1;
      xa(0) = *--x1;
      ya(0) = *--y1;

      const double* x1p = std::find(&xin[0], &xin[nXGrid], xa(1));
      const double* y1p = std::find(&yin[0], &yin[nXGrid], ya(1));
      const double* x0p = x1p - 1;
      const double* y0p = y1p - 1;

      za(0) = zin( (x0p -&xin[0]) * nXGrid + (y0p -&yin[0]) );
      za(1) = zin( (x0p -&xin[0]) * nXGrid + (y1p -&yin[0]) );
      za(2) = zin( (x1p -&xin[0]) * nXGrid + (y0p -&yin[0]) );
      za(3) = zin( (x1p -&xin[0]) * nXGrid + (y1p -&yin[0]) );

      // Column 2  
      A(0,1) = xa(0);   A(1,1) = xa(0);
      A(2,1) = xa(1);   A(3,1) = xa(1);
      // Column 3
      A(0,2) = ya(0);   A(1,2) = ya(1);
      A(2,2) = ya(0);   A(3,2) = ya(1);
      // Column 4
      A(0,3) = ya(0) * xa(0);   A(1,3) = ya(1) * xa(0);
      A(2,3) = ya(0) * xa(1);   A(3,3) = ya(1) * xa(1);
  
      fq << 1 , xou(u), you(u), xou(u)*you(u);
  
      // Rcpp::Rcout << "xa: " << xa.transpose() << ", ya: " << ya.transpose()  << ", za: " << za.transpose() << ", fq: " << fq << std::endl;
  
      result(u) = fq * A.colPivHouseholderQr().solve(za);
    }
  }
  
  return ( result ); 
}

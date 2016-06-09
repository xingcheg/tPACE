#' Print an FSVD object
#'
#' Print a simple description of an FSVD object
#'
#' @param x An FSVD object.
#' @param ... Not used.
#'
#' @examples
#' set.seed(1)
#' n <- 20
#' pts <- seq(0, 1, by=0.05)
#' sampWiener <- Wiener(n, pts)
#' sampWiener <- Sparsify(sampWiener, pts, 10)
#' res <- FSVD(sampWiener$Ly, sampWiener$Lt)
#' res
#'
#' @method print FSVD
#' @export
print.FSVD <- function(x, ...){
  obj = x;
  thisDataType <- NULL
  if(obj$optns$dataType1 == 'Dense' && obj$optns$dataType2 == 'Dense'){
    thisDataType <- 'Dense'
  } else {
    thisDataType <- 'Sparse'
  }
  if(obj$optns$dataType1 == 'DenseWithMV' && obj$optns$dataType2 == 'DenseWithMV'){
    thisDataType <- 'DenseWithMV'
  }
    
  cat("Functional Singular Value Decomposition object for", tolower(thisDataType), "data.\n\n")
  cat("The optimal number of components selected is:", length(obj$sv),"and \nthe first k (<=3) singular values are: ");
  if ( length(obj$sv) < 4) { 
    cat( round(obj$sv,3) ,"\n");
  } else {
    cat( round(obj$sv[1:3],3) ,"\n")
  }
}


 
#ifndef __OCV_THRESHOLD_HPP__
#define __OCV_THRESHOLD_HPP__

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace u {

namespace ocv {

double otsu_8u_with_mask(const cv::Mat &src, const cv::Mat &mask) {
    const int N = 256;
    int M = 0;
    int i, j, h[N] = { 0 };
    for (i = 0; i < src.rows; i++) {
        const uchar* psrc = src.ptr(i);
        const uchar* pmask = mask.ptr(i);
        for (j = 0; j < src.cols; j++) {
            if (pmask[j]) {
                h[psrc[j]]++;
                ++M;
            }
        }
    }

    double mu = 0, scale = 1. / (M);
    for (i = 0; i < N; i++)
        mu += i*(double)h[i];

    mu *= scale;
    double mu1 = 0, q1 = 0;
    double max_sigma = 0, max_val = 0;

    for (i = 0; i < N; i++) {
        double p_i, q2, mu2, sigma;

        p_i = h[i] * scale;
        mu1 *= q1;
        q1 += p_i;
        q2 = 1. - q1;

        if (std::min(q1, q2) < FLT_EPSILON || std::max(q1, q2) > 1. - FLT_EPSILON)
            continue;

        mu1 = (mu1 + i*p_i) / q1;
        mu2 = (mu - q1*mu1) / q2;
        sigma = q1*q2*(mu1 - mu2)*(mu1 - mu2);
        if (sigma > max_sigma) {
            max_sigma = sigma;
            max_val = i;
        }
    }
    return max_val;
}

double threshold_with_mask(cv::InputArray _src, cv::OutputArray _dst,
                           double thresh, double maxval, int type,
                           cv::InputArray _mask, double max_thresh=0) {
    cv::Mat src = _src.getMat();
    CV_Assert(src.channels() == 1);
    _dst.create(src.size(), src.type());
    cv::Mat dst = _dst.getMat();
    dst.setTo(0);
    CV_Assert(src.data != dst.data);  // no inplace processing
    cv::Mat mask = _mask.getMat();

    if (mask.empty() || (mask.rows == src.rows &&
                         mask.cols == src.cols &&
                         cv::countNonZero(mask) == src.rows * src.cols)) {
        // if empty mask, or all-white mask, use cv::threshold
        thresh = cv::threshold(src, dst, thresh, maxval, type);
    } else {
        // use mask
        bool use_otsu = (type & cv::THRESH_OTSU) != 0;
        if (use_otsu) {
            // if OTSU, get thresh value on mask only
            thresh = otsu_8u_with_mask(src, mask);
            if (max_thresh > 0 && thresh > max_thresh) {
                thresh = max_thresh;
            }
            // remove THRESH_OTSU from type
            type &= cv::THRESH_MASK;
        }

        // apply cv::threshold on all image
        thresh = cv::threshold(src, dst, thresh, maxval, type);
    }
    return thresh;
}

void niblack_threshold(cv::InputArray _src, cv::OutputArray _dst, double maxValue,
                       int type, int blockSize=5, double delta=-0.1) {
    // input grayscale image
    cv::Mat src = _src.getMat();
    CV_Assert(src.channels() == 1);
    CV_Assert(blockSize % 2 == 1 && blockSize > 1);
    type &= cv::THRESH_MASK;

    // compute local threshold (T = mean + k * stddev)
    // using mean and standard deviation in the neighborhood of each pixel
    // (intermediate calculations are done with floating-point precision)
    cv::Mat thresh;
    {
        // note that: Var[X] = E[X^2] - E[X]^2
        cv::Mat mean, sqmean, stddev;
        cv::boxFilter(src, mean, CV_32F, cv::Size(blockSize, blockSize),
        cv::Point(-1, -1), true, cv::BORDER_REPLICATE);
        cv::sqrBoxFilter(src, sqmean, CV_32F, cv::Size(blockSize, blockSize),
        cv::Point(-1, -1), true, cv::BORDER_REPLICATE);
        cv::sqrt(sqmean - mean.mul(mean), stddev);
        thresh = mean + stddev * static_cast<float>(delta);
        thresh.convertTo(thresh, src.depth());
    }

    // prepare output image
    _dst.create(src.size(), src.type());
    cv::Mat dst = _dst.getMat();
    dst.setTo(0);
    CV_Assert(src.data != dst.data);  // no inplace processing

    // apply thresholding: ( pixel > threshold ) ? foreground : background
    cv::Mat mask;
    switch (type) {
        case cv::THRESH_BINARY:      // dst = (src > thresh) ? maxval : 0
        case cv::THRESH_BINARY_INV:  // dst = (src > thresh) ? 0 : maxval
             cv::compare(src, thresh, mask, (type == cv::THRESH_BINARY ? cv::CMP_GT : cv::CMP_LE));
             dst.setTo(0);
             dst.setTo(maxValue, mask);
             break;
        case cv::THRESH_TRUNC:       // dst = (src > thresh) ? thresh : src
             cv::compare(src, thresh, mask, cv::CMP_GT);
             src.copyTo(dst);
             thresh.copyTo(dst, mask);
             break;
        case cv::THRESH_TOZERO:      // dst = (src > thresh) ? src : 0
             case cv::THRESH_TOZERO_INV:  // dst = (src > thresh) ? 0 : src
             cv::compare(src, thresh, mask, (type == cv::THRESH_TOZERO ? cv::CMP_GT : cv::CMP_LE));
             dst.setTo(0);
             src.copyTo(dst, mask);
             break;
        default:
             CV_Error(CV_StsBadArg, "Unknown threshold type");
             break;
    }
}

double hierarchy_threshold_rect(const cv::Mat &src, cv::Mat &dst,
                                const cv::Rect &roi, double thresh=127.0,
                                double maxval=0,
                                int type=cv::THRESH_BINARY|cv::THRESH_OTSU,
                                double max_thresh=0) {
    cv::Mat mask = cv::Mat::zeros(src.size(), CV_8UC1);
    cv::rectangle(mask, roi, cv::Scalar(255), cv::FILLED);
    cv::Mat tmp = src.clone();
    tmp.setTo(cv::Scalar(0), 255-mask);
    return threshold_with_mask(tmp, dst, thresh, maxval, type, mask, max_thresh);
}

void hierarchy_threshold(cv::InputArray _src, cv::OutputArray _dst,
                         int thresh, int type, double maxval=255,
                         double max_thresh=0, int min_area=0) {
    cv::Mat src = _src.getMat();
    CV_Assert(src.channels() == 1);

    _dst.create(src.size(), src.type());
    cv::Mat dst = _dst.getMat();
    dst.setTo(0);
    CV_Assert(src.data != dst.data);  // no inplace processing

    cv::Mat mask;
    cv::threshold(src, mask, thresh, 255, cv::THRESH_BINARY|cv::THRESH_OTSU);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (size_t i=0; i<contours.size(); ++i) {
        cv::Rect r = cv::boundingRect(contours[i]);
        int area = r.width * r.height;
        if (area > min_area) {
            cv::Mat bin;
            hierarchy_threshold_rect(src, bin, r, thresh, maxval,
                                     type|cv::THRESH_OTSU, max_thresh);
            dst.setTo(cv::Scalar(255), bin);
        }
    }
}

}

}

#endif

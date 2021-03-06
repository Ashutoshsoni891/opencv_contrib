// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.
#ifndef OPENCV_TEST_PRECOMP_HPP
#define OPENCV_TEST_PRECOMP_HPP

#include <chrono>
#include <opencv2/core.hpp>
#include <opencv2/ts.hpp>
#include <opencv2/ts/ocl_test.hpp>  // OCL_ON, OCL_OFF
#include <opencv2/imgcodecs.hpp>
#include <opencv2/quality.hpp>
#include <opencv2/quality/quality_utils.hpp>

namespace opencv_test
{
namespace quality_test
{

const cv::String
    dataDir = "cv/optflow/"
    , testfile1a = dataDir + "rock_1.bmp"
    , testfile1b = dataDir + "rock_2.bmp"
    , testfile2a = dataDir + "RubberWhale1.png"
    , testfile2b = dataDir + "RubberWhale2.png"
    ;

const cv::Scalar
    MSE_EXPECTED_1 = { 2136.0525 } // matlab: immse('rock_1.bmp', 'rock_2.bmp') == 2.136052552083333e+03
    , MSE_EXPECTED_2 = { 92.8235, 109.4104, 121.4 } // matlab: immse('rubberwhale1.png', 'rubberwhale2.png') == {92.8235, 109.4104, 121.4}
    ;

inline cv::Mat get_testfile(const cv::String& path, int flags = IMREAD_UNCHANGED )
{
    auto full_path = TS::ptr()->get_data_path() + path;
    auto result = cv::imread( full_path, flags );
    if (result.empty())
        CV_Error(cv::Error::StsObjectNotFound, "Cannot find file: " + full_path );
    return result;
}

inline cv::Mat get_testfile_1a() { return get_testfile(testfile1a, IMREAD_GRAYSCALE); }
inline cv::Mat get_testfile_1b() { return get_testfile(testfile1b, IMREAD_GRAYSCALE); }
inline cv::Mat get_testfile_2a() { return get_testfile(testfile2a); }
inline cv::Mat get_testfile_2b() { return get_testfile(testfile2b); }
inline std::vector<cv::Mat> get_testfile_1a2a() { return { get_testfile_1a(), get_testfile_2a() }; }
inline std::vector<cv::Mat> get_testfile_1b2b() { return { get_testfile_1b(), get_testfile_2b() }; }

const double QUALITY_ERR_TOLERANCE = .001  // allowed margin of error
    ;

inline void quality_expect_near( const cv::Scalar& a, const cv::Scalar& b, double err_tolerance = QUALITY_ERR_TOLERANCE)
{
    for (int i = 0; i < a.rows; ++i)
    {
        if (std::isinf(a(i)))
            EXPECT_EQ(a(i), b(i));
        else
            EXPECT_NEAR(a(i), b(i), err_tolerance);
    }
}

// execute quality test for a pair of images
template <typename TMat>
inline void quality_test(cv::Ptr<quality::QualityBase> ptr, const TMat& cmp, const Scalar& expected, const std::size_t quality_maps_expected = 1)
{
    std::vector<cv::Mat> qMats = {};
    ptr->getQualityMaps(qMats);
    EXPECT_TRUE( qMats.empty());

    quality_expect_near( expected, ptr->compute(cmp));

    EXPECT_FALSE(ptr->empty());
    ptr->getQualityMaps(qMats);
    EXPECT_EQ( qMats.size(), quality_maps_expected);
    for (auto& qm : qMats)
    {
        EXPECT_GT(qm.rows, 0);
        EXPECT_GT(qm.cols, 0);
    }

    ptr->clear();
    EXPECT_TRUE(ptr->empty());
}

/* A/B test benchmarking for development purposes */
/*
template <typename Fn>
inline void quality_performance_test( const char* name, Fn&& op )
{
    const auto exec_test = [&]()
    {
        const int NRUNS = 100;
        const auto start_t = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < NRUNS; ++i)
            op();

        const auto end_t = std::chrono::high_resolution_clock::now();
        std::cout << name << " performance (OCL=" << cv::ocl::useOpenCL() << "): " << (double)(std::chrono::duration_cast<std::chrono::milliseconds>(end_t - start_t).count()) / (double)NRUNS << "ms\n";
    };

    // only run tests in NDEBUG mode
#ifdef NDEBUG
    OCL_OFF(exec_test());
    OCL_ON(exec_test());
#endif
}
*/
}
}

#endif
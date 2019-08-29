#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <iostream>
#include <vector>
#include <numeric> 
#include <opencv2/opencv.hpp>
#include <chrono>
#include <sstream>

#define PI 3.14159265358979323846

namespace Util
{

	inline double timespec2second (timespec& startTime, timespec& endTime){
		if (endTime.tv_nsec < startTime.tv_nsec) {
			return endTime.tv_sec - startTime.tv_sec - 1 + (endTime.tv_nsec + 1.0e+9 - startTime.tv_nsec)/1.0e9;
		} else {
			return endTime.tv_sec - startTime.tv_sec + (endTime.tv_nsec - startTime.tv_nsec)/1.0e9;
		}
	}
    
    inline double degree2rad (double degree){
		return degree / 180.0 * PI;
    }
	
	inline double rad2degree (double rad) {
        return rad * 180.0 / PI;
    }
	
	inline void checkCVMat(const cv::Mat &m1) {
		std::cout << "Checking cv::Mat ..." << std::endl;
		// 行数
		std::cout << "rows:" << m1.rows << std::endl;
		// 列数
		std::cout << "cols:" << m1.cols << std::endl;
		// 次元数
		std::cout << "dims:" << m1.dims << std::endl;
		// サイズ（2次元の場合）
		std::cout << "size[]:" << m1.size().width << "," << m1.size().height << std::endl;
		// ビット深度ID
		std::cout << "depth (ID):" << m1.depth() << "(=" << CV_64F << ")" << std::endl;
		// チャンネル数
		std::cout << "channels:" << m1.channels() << std::endl;
		// （複数チャンネルから成る）1要素のサイズ [バイト単位]
		std::cout << "elemSize:" << m1.elemSize() << "[byte]" << std::endl;
		// 1要素内の1チャンネル分のサイズ [バイト単位]
		std::cout << "elemSize1 (elemSize/channels):" << m1.elemSize1() << "[byte]" << std::endl;
		// 要素の総数
		std::cout << "total:" << m1.total() << std::endl;
		// ステップ数 [バイト単位]
		std::cout << "step:" << m1.step << "[byte]" << std::endl;
		// 1ステップ内のチャンネル総数
		std::cout << "step1 (step/elemSize1):" << m1.step1() << std::endl;
		// データは連続か？
		std::cout << "isContinuous:" << (m1.isContinuous() ? "true" : "false") << std::endl;
		// 部分行列か？
		std::cout << "isSubmatrix:" << (m1.isSubmatrix() ? "true" : "false") << std::endl;
		// データは空か？
		std::cout << "empty:" << (m1.empty() ? "true" : "false") << std::endl;

		std::cout << std::endl;
	} 
	
	inline bool includes(int min, int max, int value){
			return min <= value && value <= max;
	}
	
	inline cv::Point2d cartesian(cv::Point2d cvCoord){
		return cv::Point2d(cvCoord.x, -cvCoord.y);
	}

	inline double average(std::vector<double> vec){
		return std::accumulate(vec.begin(), vec.end(), 0.0) / vec.size();
	}

	inline void exportMatToCSV(const cv::Mat mat){
		assert(mat.channels() == 1);

		std::ofstream ofs("mat.csv");

		for(int y=0;y<mat.rows;y++){
			for(int x=0;x<mat.cols;++x){
				ofs << mat.at<float>(y, x) << ",";
			}
			ofs << std::endl;
		}
		ofs.close();
	}

	inline void exportMatToCSV(const cv::Mat mat, const char* const fileName){
		assert(mat.channels() == 1);

		std::ofstream ofs(fileName);

		for(int y=0;y<mat.rows;y++){
			for(int x=0;x<mat.cols;++x){
				ofs << mat.at<float>(y, x) << ",";
			}
			ofs << std::endl;
		}
		ofs.close();
	}


	
};


/* How to use:
    Bench::bench([&]{
        // process to bench
        // outputs elapsed time in milliseconds, like 30 [ms]
    });

    Bench::bench<std::chrono::milliseconds>([&]{
        // process to bench
        // outputs elapsed time in milliseconds, like 40 [ms]
    });

    Bench::bench<std::chrono::microseconds>([&]{
        // process to bench
        // outputs elapsed time in microseconds, like 50 [us]
        // Note: In VC2015+, std::chrono::high_resolution_clock is equivalent to QueryPerformanceCounter
    });

    Overhead of bench is at most 20 us in MacPro Late 2013 (3.7GHz Quad-Core Intel Xeon E5)
 */

namespace Bench {
    template <typename TimeT = std::chrono::milliseconds, typename F>
    inline TimeT take_time(F &&f) {
        const auto begin = std::chrono::high_resolution_clock::now();
        f();
        const auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<TimeT>(end - begin);
    }

    template<typename TimeT, typename F> struct BenchDelegate {
        static long delegatedBenchFunc(F&& f){
            const auto t = take_time<TimeT>(std::forward<F>(f));
            std::chrono::duration<long, std::milli> t_ = t;
            printf("%ld [ms]\n", t_.count());
            return t_.count();
        }
    };

    template<typename F> struct BenchDelegate<std::chrono::milliseconds, F> {
        static long delegatedBenchFunc(F&& f){
            const auto t = take_time<std::chrono::milliseconds>(std::forward<F>(f));
            std::chrono::duration<long, std::milli> t_ = t;
            printf("%ld [ms]\n", t_.count());
            return t_.count();
        }
    };

    template<typename F> struct BenchDelegate<std::chrono::microseconds, F> {
        static long delegatedBenchFunc(F&& f){
            const auto t = take_time<std::chrono::microseconds>(std::forward<F>(f));
            std::chrono::duration<long, std::micro> t_ = t;
            printf("%ld [us]\n", t_.count());
            return t_.count();
        }
    };

    template <typename TimeT = std::chrono::milliseconds, typename F>
    inline long bench(F &&f) {
        return BenchDelegate<TimeT, F>::delegatedBenchFunc(std::forward<F>(f));
    }

    template <typename charT = char, typename T>
    inline std::basic_string<charT> to_s(T v) {
        std::basic_stringstream<charT> ss;
        ss << v;
        return ss.str();
    }
}


#endif

/*
Copyright (c) 2016 HPC Department of SenseTime Group Limited.

*************************************************************************************
商汤集团（SenseTime Group）是一家科技创新公司，致力于引领人工智能核心“深度学习”
技术突破，构建人工智能、大数据分析行业解决方案。我们成功聚集了当下华人中最出色、
最有影响力的深度学习、计算机视觉科学家，以及来自于谷歌、百度、微软、联想等一批
产业界的领军人物。在人工智能产业兴起的大背景下，商汤集团凭借在技术、人才、专利
上超过十年的积累，迅速成为了人工智能行业领军企业。

公司的核心团队是由两部分精英组成：一部分是在世界各所顶尖大学，如麻省理工学院、
斯坦福大学、清华大学、香港大学、香港中文大学、香港科技大学取得博士学位的优秀科学家；
另一部分是来自谷歌、微软、联想、百度等产业界的资深员工。学界业界精英的融合，
成就了SenseTime人才储备上的优势，为公司的长久发展打下了坚实基础。
加入我们，一起做中国的原创技术。

rogy.liu@sensetime.com
*************************************************************************************
*/
#ifndef TIME_HELPER_H_8FVB3L0U
#define TIME_HELPER_H_8FVB3L0U

#ifndef DISABLE_TIMING
#include <ctime>
#include <cstdio>

#ifdef _MSC_VER
#define __TIC__() double __timing_start = clock()
#define __TOC__()												\
do {															\
	double __timing_end = clock();								\
	fprintf(stdout, "TIME(ms): %lf\n",							\
		(__timing_end - __timing_start)							\
				/ CLOCKS_PER_SEC * 1000);						\
} while (0)

#else
#include <unistd.h>
#include <sys/time.h>

#define __TIC__()												\
struct timeval __timing_start, __timing_end;					\
gettimeofday(&__timing_start, NULL);

#define __TOC__()												\
do {															\
	gettimeofday(&__timing_end, NULL);							\
	double __timing_gap = (__timing_end.tv_sec -				\
							__timing_start.tv_sec) *			\
							1000.0 +							\
							(__timing_end.tv_usec -				\
							__timing_start.tv_usec) /			\
							1000.0;								\
	fprintf(stdout, "TIME(ms): %lf\n", __timing_gap);			\
} while (0)

#endif

#else
#define __TIC__()
#define __TOC__()
#endif

#endif /* end of include guard: IMAGEHELPER_HPP_8FVB3L0U */

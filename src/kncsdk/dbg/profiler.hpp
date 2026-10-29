//////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright Triple Buffer Software 2003 All rights reserved                                        //
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TB_DBG_PROFILER_HPP
#define TB_DBG_PROFILER_HPP

// Comment this out if you want the calls to the profiler to do nothing.
//#define TB_USE_PROFILER

#include <unordered_map>
#include <fstream>
#include <cstdint>

namespace dbg
{
    typedef int64_t i64;

    //////////////////////////////////////////////////////////////////////////
    // Hash para wchar_t*
    //////////////////////////////////////////////////////////////////////////

    struct wchar_ptr_hash
    {
        size_t operator()(const wchar_t* str) const
        {
            size_t ret = 0;

            while (*str)
            {
                ret ^= (static_cast<size_t>(*str) << 1);
                ++str;
            }

            return ret;
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // Comparação de igualdade para wchar_t*
    //////////////////////////////////////////////////////////////////////////

    struct wchar_ptr_equal
    {
        bool operator()(const wchar_t* left, const wchar_t* right) const
        {
            while (*left && *right)
            {
                if (*left != *right)
                    return false;

                ++left;
                ++right;
            }

            return (*left == *right);
        }
    };

    class profiler
    {
        struct profile_info
        {
            float total_time;
            float max_time;
            float min_time;
            float start_time;

            i64 num_profiles;

            profile_info* parent;

            typedef std::unordered_map<
                const wchar_t*,
                profile_info*,
                wchar_ptr_hash,
                wchar_ptr_equal
            > hash_t;

            hash_t children;
        };

        static profile_info* m_root;
        static profile_info* m_current;
        static i64 m_frequency;

        static void kill_hash_map(profile_info::hash_t* pmap);
        static void output_profile_info(
            profile_info::hash_t* pmap,
            std::ofstream& outfile,
            profile_info* parent);

    public:

        profiler(const wchar_t* name);
        ~profiler();

        static void start(const wchar_t* name);
        static void end();

        static void init_profiler();
        static void kill_profiler(const wchar_t* output_file);
    };

} // namespace dbg

#ifdef TB_USE_PROFILER

#define tb_profiler_help(name, l) tb_profiler_help2(l)(name)
#define tb_profiler_help2(li) tb_profile_block##li
#define tbProfile(name) ::dbg::profiler tb_profiler_help(name, __LINE__)

#else

#define tbProfile(name) ((void)0)

#endif

#endif // TB_DBG_PROFILER_HPP

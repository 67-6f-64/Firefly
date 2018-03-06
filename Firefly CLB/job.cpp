#include "job.hpp"

namespace firefly
{
	namespace job
	{
		bool is_adventurer_warrior(int nJob)
		{
			return (nJob == 100 || nJob == 110 || nJob == 111 || nJob == 112 || nJob == 120 || nJob == 121 || nJob == 122 || nJob == 130 || nJob == 131 || nJob == 132);
		}

		bool is_adventurer_mage(int nJob)
		{
			return (nJob == 200 || nJob == 210 || nJob == 211 || nJob == 212 || nJob == 220 || nJob == 221 || nJob == 222 || nJob == 230 || nJob == 231 || nJob == 232);
		}

		bool is_adventurer_archer(int nJob)
		{
			return (nJob == 300 || nJob == 310 || nJob == 311 || nJob == 312 || nJob == 320 || nJob == 321 || nJob == 322);
		}

		bool is_adventurer_rogue(int nJob)
		{
			return (nJob == 400 || nJob == 420 || nJob == 421 || nJob == 422 || nJob == 410 || nJob == 411 || nJob == 412 || (nJob / 10) == 43);
		}

		bool is_adventurer_pirate(int nJob)
		{
			return (nJob == 500 || nJob == 510 || nJob == 511 || nJob == 512 || nJob == 520 || nJob == 521 || nJob == 522 || job::is_cannonshooter_job(nJob));
		}

		bool is_cannonshooter_job(int nJob)
		{
			return (nJob / 10 == 53) || nJob == 501;
		}

		bool is_jett_job(int nJob)
		{
			return (nJob / 10 == 57) || nJob == 508;
		}

		bool is_unknown_job_2(int nJob)
		{
			return (nJob / 1000) == 4;
		}

		bool is_cygnus_job(int nJob)
		{
			return (nJob / 1000) == 1;
		}

		bool is_resistance_job(int nJob)
		{
			return (nJob / 1000) == 3;
		}

		bool is_evan_job(int nJob)
		{
			return (nJob / 100) == 22 || nJob == 2001;
		}

		bool is_mercedes_job(int nJob)
		{
			return (nJob / 100) == 23 || nJob == 2002;
		}

		bool is_phantom_job(int nJob)
		{
			return (nJob / 100) == 24 || nJob == 2003;
		}

		bool is_leader_job(int nJob)
		{
			return (nJob / 1000) == 5;
		}

		bool is_luminous_job(int nJob)
		{
			return (nJob / 100) == 27 || nJob == 2004;
		}

		bool is_dragonborn_job(int nJob)
		{
			return (nJob / 1000) == 6;
		}

		bool is_zero_job(int nJob)
		{
			return nJob == 10000 || nJob == 10100 || nJob == 10110 || nJob == 10111 || nJob == 10112;
		}

		bool is_hidden_job(int nJob)
		{
			return (nJob / 100) == 25 || nJob == 2005;
		}

		bool is_aran_job(int nJob)
		{
			return (nJob / 100) == 21 || nJob == 2000;
		}

		bool is_kinesis_job(int nJob)
		{
			return nJob == 14000 || nJob == 14200 || nJob == 14210 || nJob == 14211 || nJob == 14212;
		}

		bool is_demon_slayer_job(int nJob)
		{
			return (nJob / 100) == 31 || nJob == 3001;
		}

		bool is_xenon_job(int nJob)
		{
			return (nJob / 100) == 36 || nJob == 3002;
		}

		bool is_beast_tamer_job(int nJob)
		{
			return (nJob / 100) == 112 || nJob == 11000;
		}
	}
}
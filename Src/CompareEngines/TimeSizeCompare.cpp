/**
 * @file  TimeSizeCompare.cpp
 *
 * @brief Implementation file for TimeSizeCompare
 */

#include "TimeSizeCompare.h"
#include <cstdlib>
#include <vector>
#include <cstdint>
#include <Poco/Timestamp.h>
#include "DiffItem.h"
#include "DiffWrapper.h"

using Poco::Timestamp;

namespace CompareEngines
{

TimeSizeCompare::TimeSizeCompare()
		: m_ignoreSmallDiff(false)
{
}

TimeSizeCompare::~TimeSizeCompare()
{
}

/**
 * @brief Set compare-type specific options.
 * @param [in] ignoreSmallDiff Ignore small time differences?
 */
void TimeSizeCompare::SetAdditionalOptions(bool ignoreSmallDiff)
{
	m_ignoreSmallDiff = ignoreSmallDiff;
}

/**
 * @brief Compare two specified files, byte-by-byte
 * @param [in] compMethod Compare method used.
 * @param [in] di Diffitem info.
 * @return DIFFCODE
 */
int TimeSizeCompare::CompareFiles(int compMethod, int nfiles, const DIFFITEM &di) const
{
	unsigned code = DIFFCODE::SAME;
	if ((compMethod == CMP_DATE) || (compMethod == CMP_DATE_SIZE))
	{
		// Compare by modified date
		// Check that we have both filetimes
		if (di.diffFileInfo[0].mtime != 0 && di.diffFileInfo[1].mtime != 0 &&
		    (nfiles < 3 || di.diffFileInfo[1].mtime != 0 && di.diffFileInfo[2].mtime != 0))
		{
			int64_t nTimeDiff   = di.diffFileInfo[0].mtime - di.diffFileInfo[1].mtime;
			int64_t nTimeDiff12 = (nfiles < 3) ? 0 : (di.diffFileInfo[1].mtime - di.diffFileInfo[2].mtime);
			if (nTimeDiff   < 0) nTimeDiff   *= -1;
			if (nTimeDiff12 < 0) nTimeDiff12 *= -1;
			if (m_ignoreSmallDiff)
			{
				// If option to ignore small timediffs (couple of seconds)
				// is set, decrease absolute difference by allowed diff
				nTimeDiff   -= SmallTimeDiff * Timestamp::resolution();
				nTimeDiff12 -= SmallTimeDiff * Timestamp::resolution();
			}
			if (nTimeDiff <= 0 && nTimeDiff12 <= 0)
				code = DIFFCODE::SAME;
			else
				code = DIFFCODE::DIFF;
		}
		else
		{
			// Filetimes for item(s) could not be read. So we have to
			// set error status, unless we have DATE_SIZE -compare
			// when we have still hope for size compare..
			if (compMethod == CMP_DATE_SIZE)
				code = DIFFCODE::SAME;
			else
				code = DIFFCODE::CMPERR;
		}
	}
	// This is actual CMP_SIZE method..
	// If file sizes differ mark them different
	if ((compMethod == CMP_DATE_SIZE) || (compMethod == CMP_SIZE))
	{
		if (di.diffFileInfo[0].size != di.diffFileInfo[1].size || 
		    (nfiles > 2 && di.diffFileInfo[1].size != di.diffFileInfo[2].size))
		{
			code = DIFFCODE::DIFF;
		}
	}
	return code;
}

} // namespace CompareEngines

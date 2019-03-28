/*
    This file is part of solidity.

    solidity is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    solidity is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with solidity.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * Unit tests for the dead code eliminator.
 */

#include <test/Options.h>

#include <test/libyul/Common.h>

#include <libyul/AsmData.h>

#include <libyul/optimiser/DeadCodeEliminator.h>


using namespace std;
using namespace langutil;

namespace yul
{
namespace test
{

namespace
{
	bool deadCode(string const& _source)
	{
		shared_ptr<Block> ast = parse(_source, false).first;
		BOOST_REQUIRE(ast);

		return DeadCodeEliminator{}.containsDeadCode(*ast);
	}
}


BOOST_AUTO_TEST_SUITE(YulDeadCode)

BOOST_AUTO_TEST_CASE(empty_code)
{
	BOOST_CHECK_EQUAL(deadCode("{}"), false);
}

BOOST_AUTO_TEST_CASE(nested_blocks)
{
	BOOST_CHECK_EQUAL(deadCode("{ {} {} {{ }} }"), false);
}

BOOST_AUTO_TEST_CASE(instruction)
{
	BOOST_CHECK_EQUAL(deadCode("{ pop(calldatasize()) }"), false);
}

BOOST_AUTO_TEST_CASE(early_revert)
{
	BOOST_CHECK_EQUAL(deadCode("{ let a := 1 revert(0,0) let b:= 2 a := b }"), true);
	BOOST_CHECK_EQUAL(deadCode("{ let a := 1 let b:= 2 a := b }"), false);
	BOOST_CHECK_EQUAL(deadCode("{ let a := 1 revert(0,0) }"), false);
}

BOOST_AUTO_TEST_CASE(early_break)
{
	BOOST_CHECK_EQUAL(deadCode("{ for { let a:=20 } lt(a, 40) {} { a:=a break mstore(0, a) } }"), true);
	BOOST_CHECK_EQUAL(deadCode("{ for { let a:=20 } lt(a, 40) {} { a:=a break } }"), false);
	BOOST_CHECK_EQUAL(deadCode("{ for { let a:=20 } lt(a, 40) {} { a:=a mstore(0, a) } }"), false);
}

BOOST_AUTO_TEST_SUITE_END()

}
}

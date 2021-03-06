/**
 * CallSite.cpp
 *
 * Copyright (c) 2014 John Sadler <deathofathousandpapercuts@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "CallSite.h"

#include <boost/foreach.hpp>

OnwardCall& CallSite::add_onward_call(CallSite* const next_site) {

	BOOST_FOREACH(OnwardCall& o, onward_calls) {

		if (o.next == next_site) {
			return o;
		}
	}

	onward_calls.push_back(OnwardCall(next_site));

	return onward_calls.back();
}


void CallSite::clear_costs() {
	times_called = 0;
	cum_alloc = 0;

	BOOST_FOREACH( OnwardCall& oc, onward_calls) {
		oc.times_called = 0;
		oc.cum_alloc = 0;
	}
}


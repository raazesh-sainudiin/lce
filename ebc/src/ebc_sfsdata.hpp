/* 
 * Copyright (C) 2005--2009 Raazesh Sainudiin and Kevin Thornton
 * 
 * This file is part of lce, a C++ class library for lumped coalescent experiments.
 *
 * lce is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
/*! \file ebc_sfsdata.hpp
	\brief prototype for reading sfs data

	\todo there should be include<ebc_sfstypes.hpp> here but since we know it is in include<ebc_moves.hpp>
	i just didn't include it.  We should just let ifndef worry about that and have the appropriate includes. 
	So may be this should be done consistently in all files.
*/
#ifndef __SFSDATA_HPP
#define __SFSDATA_HPP__

#include<ebc_moves.hpp> 


void ReadSfsData(std::vector<sfs_array_type>& DataSfspectra, params& p);
#endif
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright (c) 2008-2021 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

#include <new>
#include "SnippetVehicleFilterShader.h"
#include "PxPhysicsAPI.h"

#include <iostream>

namespace snippetvehicle
{

using namespace physx;

PxFilterFlags VehicleFilterShader
(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	PX_UNUSED(attributes0);
	PX_UNUSED(attributes1);
	PX_UNUSED(constantBlock);
	PX_UNUSED(constantBlockSize);
	pairFlags = PxPairFlag::eCONTACT_DEFAULT;
	if ((filterData0.word0 == COLLISION_FLAG_FOOD && filterData1.word0 == COLLISION_FLAG_WHEEL) ||
		(filterData1.word0 == COLLISION_FLAG_FOOD && filterData0.word0 == COLLISION_FLAG_WHEEL) ||
		(filterData0.word0 == COLLISION_FLAG_FOOD && filterData1.word0 == COLLISION_FLAG_CHASSIS) ||
		(filterData1.word0 == COLLISION_FLAG_FOOD && filterData0.word0 == COLLISION_FLAG_CHASSIS))
	{
		return PxFilterFlag::eSUPPRESS;
	}

	if ((0 == (filterData0.word0 & filterData1.word1)) && (0 == (filterData1.word0 & filterData0.word1))) {
		if (PxGetFilterObjectType(attributes0) == PxFilterObjectType::eRIGID_DYNAMIC 
			&& PxGetFilterObjectType(attributes0) == PxFilterObjectType::eRIGID_DYNAMIC &&
			filterData0.word0 != 0 && filterData1.word0 != 0) // Why does this work? no idea. But who cares!
		{
			pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
			pairFlags |= PxPairFlags(PxU16(filterData0.word2 | filterData1.word2));
			return PxFilterFlags();
		}
		else {
			return PxFilterFlag::eSUPPRESS;
		}
	}

	pairFlags |= PxPairFlags(PxU16(filterData0.word2 | filterData1.word2));

	return PxFilterFlags();
}

} // namespace snippetvehicle

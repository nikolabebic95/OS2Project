/**
* \file SlabUtility.h
* \brief File providing the interface for the slab allocator
*/

#ifndef _slabutility_h_
#define _slabutility_h_

namespace os2bn140314d {

	/**
	 * \brief Class providing interface for the slab allocator
	 */
	class Slab final {
	public:


	private:

		#pragma region Delete constructors

		Slab() = delete;
		Slab(const Slab &) = delete;
		void operator=(const Slab &) = delete;

		#pragma endregion 

	};

}

#endif

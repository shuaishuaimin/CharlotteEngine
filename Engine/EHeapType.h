#pragma once

namespace Charalotte
{
	enum class HeapType
	{
		CBVSRVUAVHeap = 0,
		SAMPLERHeap = (CBVSRVUAVHeap + 1),
		RTVHeap = (SAMPLERHeap + 1),
		DSVHeap = (RTVHeap + 1) ,
	};
}
/*
	NovaCorps - Pool.h
		William Chapman

		11/23/2018 2:27:23 PM

	This header file describes the Pool class.

		A Pool can be used to create a group of objects of any defined type. These objects are
	marked as "inactive" or "free" until  otherwise retrieved with .getNext(), where they are
	listed as "active". These objects can then be freed up again by	calling .release(object) on
	them.

		This creates a system where a 'pool' of objects which are instantiated at start-up and then
	retrieved throughout runtime as and when they are needed, rather than instantiating new objects
	at runtime (a memory intensive process).
	
		Objects will not have default settings when they are retrieved, and will instead retain
	whatever properties they had when last used.


	To iterate through a pool's actives:

		int activeObjects;
		auto objectsArray = pool.activeAddresses(activeObjects);

		if (objectsArray != nullptr) //Safety check. Alternative: if (activeObjects > 0)
		{
			for (int i = 0; i < activeObjects; i++)
			{
				//code to be run on objectsArray[i]
			}
		}

*/


#ifndef POOL_H

	#define POOL_H

	template <class type>
	class Pool
	{
		//Private members
		private:

			//The Size of the pool [default 10]
			int m_iSize = 10;

			//Pointer to the current position of the first free GameObject in the pool
			int m_iNextFreePosition = 0;
			
			//Holds the pointer to the start of our array of pointers to objects in the pool
			type** m_pArrayLocation;


		//Public members
		public:

			//Creates a Pool of a_size with default objects of given type
			Pool()
			{
				//Create pool array on the heap so it can be deleted when pool is resized or deleted
				m_pArrayLocation = new type*[m_iSize];

				//Create new objects on the heap and reference a pointer to it in our pool array (which is also on the heap)
				for (int i = 0; i < m_iSize; i++)
				{
					//Create an empty object and point to it
					type* l_pObject = new type();

					//Set the next pointer in our array of pointers (pool) to this pointer (pointing to a new blank object)
					m_pArrayLocation[i] = l_pObject;
				}
			}
			
			//Creates a Pool of a_size with default objects of given type
			Pool(const int a_iSize)
			{
				if (a_iSize > 0)
				{
					//Define size property
					m_iSize = a_iSize;
					
					//Create pool array on the heap so it can be deleted when pool is resized or deleted
					m_pArrayLocation = new type*[a_iSize];

					//Create new objects on the heap and reference a pointer to it in our pool array (which is also on the heap)
					for (int i = 0; i < a_iSize; i++)
					{
						//Create an empty object and point to it
						type* l_pObject = new type();

						//Set the next pointer in our array of pointers (pool) to this pointer (pointing to a new blank object)
						m_pArrayLocation[i] = l_pObject;
					}
				}
				else
				{
					//throw std::range_error(__FILE__ ": <Pool Error>: Pool must have size greater than 0");
					delete this;
				}
			}
			
			//Creates a Pool of a_size classes cloned from the given class GameObject texture
			Pool(type* a_pObjectToPool, const int a_iSize)
			{
				if (a_iSize > 0)
				{
					//Define size property
					m_iSize = a_iSize;
					
					//Create pool array on the heap so it can be deleted when pool is resized or deleted
					m_pArrayLocation = new type*[a_iSize];

					//Create clones of original object on the heap and reference a pointer to it in our pool array (which is also on the heap)
					for (int i = 0; i < a_iSize; i++)
					{
						//Create an empty object and point to it
						type* l_pClone = new type();

						//Set the contents of the clone pointer to the contents of the original object (thus cloning it)
						*l_pClone = *a_pObjectToPool;

						//Set the next item in our array of pointers (pool) to this pointer (pointing to a clone of the object)
						m_pArrayLocation[i] = l_pClone;
					}
				}
				else
				{
					//throw std::range_error(__FILE__ ": <Pool Error>: Pool must have size greater than 0");
					delete this;
				}
			}
			
			//Destructor
			virtual ~Pool()
			{
				//Delete pool
				for (int i_pointer = 0; i_pointer < m_iSize; i_pointer++)
				{
					delete m_pArrayLocation[i_pointer];
				}
				delete m_pArrayLocation;
			}


			//Retrieves the next object in the pool
			type* getNext()
			{
				//If we have something free
				if (m_iNextFreePosition < m_iSize)
				{
					//Get address of object located at next pointer
					const int i_positionPointer = m_iNextFreePosition;

					//Increase pointer, note: if it's now the size of the array then there aren't any left
					m_iNextFreePosition++;

					//Return address of object located at the old pointer
					return m_pArrayLocation[i_positionPointer];
				}
				else
				{
					//throw std::overflow_error(__FILE__ ": <Pool Error>: No available objects left in pool. Try releasing some objects");
					return nullptr;
				}
			}


			//Releases the object at the given address from use and internally resorts the pool to keep active and free in separate halves
			void release(type* a_pAddress)
			{
				//To hold the position of the given address in the array, if and when we find it
				int i_addressPositionInArray = -1;

				//Loop from start to finish to find location of this address in the array
				for (int i_location = 0; i_location < m_iSize; i_location++)
				{
					if (m_pArrayLocation[i_location] == a_pAddress)
					{
						i_addressPositionInArray = i_location;
						break;
					}
				}

				//If found then sort array, otherwise throw an exception
				if (i_addressPositionInArray > -1 && i_addressPositionInArray < m_iNextFreePosition)
				{					
					//Make sure where we're slotting the released object into is valid
					int lastActive = m_iNextFreePosition - 1;
					if (lastActive < 0)
					{
						//throw std::range_error(__FILE__ ": <Pool Error>: Released object can never be out of pool scope");
					}

					//Swap contents of this array address and last active array address. This sorts array into half active, half free
					type* lastActiveAddress = m_pArrayLocation[lastActive];
					m_pArrayLocation[lastActive] = a_pAddress;
					m_pArrayLocation[i_addressPositionInArray] = lastActiveAddress;

					//Decrement the pointer to the next active object, which will point it to this newly released object as it is now first in the list of free objects
					m_iNextFreePosition--;

				}
				else
				{
					//throw std::range_error(__FILE__ ": <Pool Error>: Given Address was not found in active pool or was already inactive");
				}
			}


			//Getter for size of Pool
			int size() const
			{
				return m_iSize;
			}

			//Setter for size of pool, returns true if a new array could be created (valid size) and false if it couldn't
			bool size(int a_iNewSize)
			{
				if (a_iNewSize > 0)
				{
					//create new array of a_newSize
					type** l_pNewArray = new type*[a_iNewSize];

					//more efficient at runtime to have these as a separate loops rather than make a longer loop with if statements inside

					//add elements from start of existing array to new array, up to a_newSize
					for (int i = 0; i < a_iNewSize && i < m_iSize; i++)
					{
						l_pNewArray[i] = m_pArrayLocation[i];
					}

					//operates if a_iNewSize > m_iSize
					//fill remaining elements if there are any (a_newSize-oldSize) to the rest, as a clone of the last original element
					for (int i = m_iSize; i < a_iNewSize; i++)
					{
						//Create an empty object and point to it
						type* l_pClone = new type();

						//Set the contents of the clone pointer to the contents of the last original object
						*l_pClone = *m_pArrayLocation[m_iSize-1];

						//Set the next item in our array of pointers (pool) to this pointer (pointing to a clone of the object)
						l_pNewArray[i] = l_pClone;
					}

					//operates if a_iNewSize < m_iSize
					//delete hanging elements if there are any
					for (int i = a_iNewSize; i < m_iSize; i++)
					{
						delete m_pArrayLocation[i];
					}

					//delete old array
					delete m_pArrayLocation;

					//set array pointer to point to this new array
					m_pArrayLocation = l_pNewArray;

					//Set next item pointer to end of array if array is smaller than the pointer
					if (m_iNextFreePosition > a_iNewSize) m_iNextFreePosition = a_iNewSize;

					//redefine size property
					m_iSize = a_iNewSize;

					return true;

				}

				//throw std::range_error(__FILE__ ": <Pool Error>: Pool must have size greater than 0");
				return false;

			}


			//Returns number of active elements in pool
			int activeCount()
			{
				/*
				 If the next free one is at the top (0) we have 0 active ones. And so on.
				 Potential security issue?
				 */
				return m_iNextFreePosition;
			}


			//Returns number of free elements in pool
			int freeCount()
			{
				/*
				 We have x elements. If the pointer was at 0, x-0 = number of free elements.
				 If we know the size we can invert this and get next free position. Potential security issue?
				 */
				return m_iSize - m_iNextFreePosition;
			}


			//Returns pointer to an array of addresses of all active elements in pool.
			//Useful if you've forgotten some things that need to be released.
			type** activeAddresses(int* a_end)
			{
				//Rather than make a new array and point to it, simply return a pointer to our array but with an end stop
				if (a_end != nullptr) *a_end = m_iNextFreePosition;
				return m_pArrayLocation;

				/*
				//If the next free position is 0, or at the top, then there are no active addresses
				if (m_iNextFreePosition > 0)
				{
					//You can't return an entire array from a method, and even if you could that'd be awfully intensive.
					//Instead, we'll return a pointer to the array we create. Has to be on the heap because const sizes for implicit arrays suck.

					//Make a new one
					auto m_pActivesArray = new type*[m_iNextFreePosition];

					//Go through the active ones
					for (int i = 0; i < m_iNextFreePosition; i++)
					{
						//Set the value of Nth element of our activeAddresses array to the Nth element of the array of addresses
						m_pActivesArray[i] = m_pArrayLocation[i];
					}

					//return address of the array
					return m_pActivesArray;

				}
				else
				{
					//throw std::underflow_error(__FILE__ ": <Pool Error>: Could not create array of addresses as there were no active elements");
					return nullptr;
				}
				*/
			}


	};


#endif
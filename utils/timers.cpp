// Copyright (c) 1994-2009 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(tm).

// FalconView(tm) is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// FalconView(tm) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(tm).  If not, see <http://www.gnu.org/licenses/>.

// FalconView(tm) is a trademark of Georgia Tech Research Corporation.



#include "stdafx.h"

#include "utils.h"
      


IMPLEMENT_DYNAMIC(FVW_Timer, CObject)
// The following compile if not commented out, why?
//ughIMPLEMENT_DYNAMIC(DTEDTimer, FVW_Timer)
//ughIMPLEMENT_DYNAMIC(TipTimer, FVW_Timer)
//ughIMPLEMENT_DYNAMIC(DataSourceTimer, FVW_Timer)

//--------------------- Static Timer List ---------------------

static TimerList timer_list;

TimerList* UTL_get_timer_list(void) 
{
  return &timer_list;
}

//------------------------ Timer ------------------------------    

FVW_Timer::FVW_Timer(int period)
{
   m_period=period;
   m_stopped=TRUE;
   m_counter=0;
}

                  
void FVW_Timer::tick()
{    
   if (m_stopped==FALSE)
   {
     m_counter+=TICK_PERIOD;
     if (m_counter >= m_period)
     {
        stop();      
        reset();
        expired();   //timer expired - do something
     }
   }
}
 

//------------------------ Timer List ------------------------------    
TimerList::TimerList()
{
    //hopefully the list is already NULL
}


TimerList::~TimerList()
{
   //INFO_report("begin ~TimerList()");
   delete_all();
}



void TimerList::delete_all(void)
{
  //removes and destroys all elements in the list
   POSITION pos = m_list.GetHeadPosition();
   while (pos)
   {
      //INFO_report("deleting timer");
      delete m_list.GetNext(pos);
   }
   //remove pointers from list
   m_list.RemoveAll();
}



/*
int TimerList::reset()
{
   m_position=NULL;  //set postion in list to NULL
   remove_all();  //Remove and destroy all elements curently in list
   initialize();  //initialize from ini file

   return 1;  //NEED TO RETURN CORRECT VALUE
}
*/

/*

int TimerList::initialize() 
{
   int i,j;
   CString entry,number;
   DataSource *data_source;
   BOOL done;

   if (m_list.IsEmpty() ==0)  //if list is not empty...
   {
      ERR_report("attempt to initialize non_empty list");
      return FAILURE;
   }
   
       
   //m_position=NULL;  //set postion in list to NULL
  
}     
    
return SUCCESS;
} 

*/

POSITION TimerList::get_head_position(void)
{
  
  if (m_list.IsEmpty())             // if empty...
     return NULL;                   

  return m_list.GetHeadPosition();  // get head position
}   

/*
FVW_Timer* TimerList::get_first(POSITION &pos)  
{
   //works like CList

   FVW_Timer* timer;
                
   if (!m_list.IsEmpty()) // if is not empty...
   {
      pos = m_list.GetHeadPosition();  // get head position
      timer = m_list.GetNext(pos);     // get first element
   }
   else
   {
      pos = NULL;       //set position to NULL 
      timer = NULL;     //set timer to NULL
   }

   return timer;
}
*/

FVW_Timer* TimerList::get_next(POSITION &pos)
{
   //return next element in list
   if (pos != NULL)  
     return m_list.GetNext(pos);  
   
   return NULL;
}   

/*
FVW_Timer* TimerList::get_first_of_type(POSITION &pos, const CRuntimeClass * Class)  
{
   //works like CList
   boolean_t found=FALSE;
   FVW_Timer *timer = NULL;
   pos = NULL;

   if (!m_list.IsEmpty()) // if is not empty...
   {
      pos = m_list.GetHeadPosition();  // get head position
      get_next_of_type(pos, Class);          // get next of type  
   }
   
   return timer;
}
*/

FVW_Timer* TimerList::get_next_of_type(POSITION &pos, const CRuntimeClass * Class)  
{
   //works like CList
   
   boolean_t found=FALSE;                    // not found
   FVW_Timer *timer = NULL;                  
   
   while (pos && !found) 
   {
       timer = m_list.GetNext(pos);    // get first element
       if (timer->IsKindOf(Class))           // if is of desired class...
         found=TRUE;                         // found it
   }
   
   return timer;
}



POSITION TimerList::add(FVW_Timer* timer)  
{
   return m_list.AddTail(timer);
}  


void TimerList::tick(void)  //make all elements in list tick
{
   POSITION pos;
   FVW_Timer *timer;

   pos = m_list.GetHeadPosition();
   while(pos)
   {
     timer = m_list.GetNext(pos);
     timer->tick();
   }
}








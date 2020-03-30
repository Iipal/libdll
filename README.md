# libdll
Header-only library for work with Double Linked Lists(or Bidirectional Lists)

With this library you can do:
- Create Double Linked List
- Create\Add new object to head\end of list
- Delete last\fisrt(head) object in list
- Find\Delete list object by index from head\end
- Find\Delete list object by specified data via handler function from head\end
- Print all objects data from head\end via handler function
- Set-up internal bits mask for each object(for only 1 flag for now)

Specify bevahior for list:
- DLL_GBIT_DFLT: default behavior
- DLL_GBIT_QUIET: Do not print output order and objects count in dll_print*

Specify bevahior for each list object:
- DLL_GBIT_DFLT: default behavior
- DLL_GBIT_QUIET: if object handler returns neg.value - do not print error message(dll_delkey*, dll_print*)
- DLL_GBIT_EIGN: if object handler returns neg.value - do not stop processing list and no erroo message(dll_delkey*, dll_print*)

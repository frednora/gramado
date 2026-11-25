Probing mouse presence.
Using Copilot:

Below is one approach to build a mouse-probing routine that “listens” for the mouse coordinates, traverses your global window list in reverse (so that top‐most windows are checked first), and then—if a candidate is found—further checks if there are any child windows (like title bar buttons or client area controls) covering that point.

In our example, we use two helper routines:

wm_probe_window_at() Uses the global linked list (built by your workers) and reverses the order into a temporary array to start checking from the top.

wm_probe_child_window() Given a parent window and the current mouse coordinates, it scans the global array (or you could instead scan a maintained list) for windows whose parent_window pointer equals the candidate parent. It then returns the top-most child (if any) that contains the point. For extra flexibility, it even recurses so that nested children are caught.

Below is sample code with detailed comments:

// Assume WINDOW_COUNT_MAX is defined somewhere globally.
// Assume __root_window is the pointer to the root window.

//
// Helper: Probe for a child window within a given parent window.
// This routine scans windowList[] for any window whose parent_window equals 'parent'
// and returns the top-most child in terms of z-order that contains the (x,y) point.
//
// Optionally, you can add recursion here if child windows might themselves
// have further nested children.
//
// You may also consider building a temporary array of children if ordering is important.
//
struct gws_window_d *wm_probe_child_window(struct gws_window_d *parent, int x, int y) {
    struct gws_window_d *result = NULL;
    struct gws_window_d *childList[WINDOW_COUNT_MAX];
    int childCount = 0;
    int i;
    
    // Scan the global array looking for child windows of the specified parent.
    for (i = 0; i < WINDOW_COUNT_MAX; i++) {
        struct gws_window_d *w = (struct gws_window_d *) windowList[i];
        if (w != NULL) {
            // Check that w is valid and that its parent pointer equals the given parent.
            if (w->magic == 1234 && w->used == TRUE && w->parent_window == parent) {
                // Only consider visible windows.
                if (w->state == WINDOW_STATE_MINIMIZED)
                    continue;
                childList[childCount++] = w;
            }
        }
    }
    
    // Iterate the temporary child list in reverse order
    // so that the top-most (the one added later) is checked first.
    for (i = childCount - 1; i >= 0; i--) {
        struct gws_window_d *w = childList[i];
        if (w) {
            if ( x >= w->vs.left && y >= w->vs.top &&
                 x < w->vs.right && y < w->vs.bottom ) {
 
                // If you expect nested children, you can recursively probe here.
                struct gws_window_d *nested = wm_probe_child_window(w, x, y);
                if (nested)
                    return nested;
 
                return w;
            }
        }
    }
    
    return result; // returns NULL if no child covers the point.
}

//
// Primary routine: Probe the global window list for the window under the given (x,y) point.
// It builds a temporary array from the global linked list (first_window) to traverse in reverse (top-down).
// If a candidate window is found, it then checks for child windows (controls in the titlebar or client area).
//
struct gws_window_d *wm_probe_window_at(int x, int y) {
    struct gws_window_d *tmpList[WINDOW_COUNT_MAX];
    int count = 0;
    struct gws_window_d *p = first_window; 

    // Build a temporary array from the linked list defined by first_window. 
    // Note: The linked list was built from bottom-to-top.
    while (p != NULL && count < WINDOW_COUNT_MAX) {
         tmpList[count++] = p;
         p = p->next;
    }
    
    // Traverse the temporary array in reverse order: from top-most to bottom.
    for (int i = count - 1; i >= 0; i--) {
         struct gws_window_d *w = tmpList[i];
         
         // Validation check.
         if (w->used != TRUE || w->magic != 1234)
             continue;
         if (w->state == WINDOW_STATE_MINIMIZED)
             continue;
         
         // Check if the mouse coordinates (x, y) lie inside this window.
         if ( x >= w->vs.left && y >= w->vs.top &&
              x < w->vs.right && y < w->vs.bottom ) {
              
              // Found a matching window.
              // Now, check if this window has child windows (controls, client area buttons, etc.)
              struct gws_window_d *child = wm_probe_child_window(w, x, y);
              if (child != NULL)
                  return child;
 
              // If no child window is found that covers (x,y), return this window.
              return w;
         }
    }
    
    // If nothing matches, fall back to the root window.
    return __root_window;
}


===========================

Explanation
Temporary Array from the Global List: Since your linked list is built from bottom-to-top (with first_window at the bottom and the last-added window at the end), we copy pointers from the list into a temporary array. This lets us easily iterate from the end of the array (the top-most window) down to the bottom. This ensures that if multiple windows overlap the mouse pointer, the one visually on top is detected first.

Window Validations: For each candidate window, we verify that it’s valid using its magic field, is in use, and is not minimized. We then check if the mouse coordinates fall within its boundary (using the window’s vs.left, vs.top, vs.right, and vs.bottom values).

Child Window Probe: If a candidate window is located, before immediately returning it we call wm_probe_child_window(). This helper scans for any child windows (based on the parent_window pointer) that might intercept the event. For instance, if you have title bar buttons or client area controls inside a WT_OVERLAPPED window, this helper ensures they get priority if they cover the mouse point.

Fallback: If no window in the list covers the point, the function finally returns the root window (__root_window).

=======

Next Steps
Integration with Events: This routine can be called during your event handling (for example, upon a mouse move or button click) to determine which window should receive the event.

Optimization Considerations: If the number of windows is very large, you might consider maintaining a separate ordered list (or even indexing children by parent) to avoid scanning the entire global array.

Hierarchical Improvements: If you later decide to maintain explicit child lists within each window, the recursive portion of wm_probe_child_window() can be simplified by iterating over those lists directly.

===========================

// Revised by Copilot.
// Insert a window at the bottom of the list (i.e. as the first element).
void wm_add_window_to_bottom(struct gws_window_d *window) {
    if (window == NULL) return;
    if (window->magic != 1234) return;
    if (window == first_window) return;

    // Insert at the beginning.
    window->next = first_window;
    first_window = window;
}

// Revised by Copilot.
// Append a WT_OVERLAPPED window at the top (end) of the list.
void wm_add_window_to_top(struct gws_window_d *window) {
    struct gws_window_d *Next;

    // Validate the window.
    if (window == NULL)                          return;
    if (window->used != TRUE)                      return;
    if (window->magic != 1234)                     return;
    if (window->type != WT_OVERLAPPED)             return;

    // If the list is empty or the first window is invalid, initialize it.
    if (first_window == NULL || first_window->used != TRUE || first_window->magic != 1234) {
        first_window = window;
        last_window = window;
        window->next = NULL;
        set_active_window(window);
        return;
    }

    // Traverse the list to find the last element.
    Next = first_window;
    while (Next->next != NULL) {
        Next = Next->next;
    }

    // Append the new window.
    Next->next = window;
    window->next = NULL;
    
    // Update the global last window pointer and mark the window as active.
    last_window = window;
    set_active_window(window);
}

// Revised by Copilot.
// Rebuild the global window list from the windowList array.
void wm_rebuild_list(void) {
    struct gws_window_d *window;
    
    // Reset the global linked list.
    first_window = NULL;
    last_window = NULL;
    
    for (int i = 0; i < WINDOW_COUNT_MAX; i++) {
        window = (struct gws_window_d *) windowList[i];
        if (window && window->magic == 1234 && window->type == WT_OVERLAPPED) {
            wm_add_window_to_top(window);
        }
    }
}

============================

The code below includes two functions:
wm_probe_child_window recursively checks a parent window’s children.
wm_probe_window_at starts at the top-most window 
(using last_window and following prev pointers) and returns the first window 
that contains the mouse coordinates. If that window contains children, 
it defers to wm_probe_child_window.

// Probes among the children of a given parent window for one that contains (x,y).
// This routine traverses the children (using the child_list pointer, then the sibling list) 
// and recurses so that deeply nested controls are found.
struct gws_window_d *wm_probe_child_window(struct gws_window_d *parent, int x, int y) {
    if (parent == NULL)
        return NULL;

    struct gws_window_d *child = parent->child_list;
    while (child != NULL) {
        // Validate the child.
        if (child->used == TRUE &&
            child->magic == 1234 &&
            child->state != WINDOW_STATE_MINIMIZED) {
                
            // Check if the point (x,y) is within the child's window rectangle.
            // You can use child->rcWindow or use relative coordinates from child's left/high etc.
            if (x >= child->rcWindow.left && x < child->rcWindow.right &&
                y >= child->rcWindow.top  && y < child->rcWindow.bottom) {
                    
                // Check recursively: if this child has its own children, probe them.
                struct gws_window_d *nested = wm_probe_child_window(child, x, y);
                if (nested != NULL)
                    return nested;
                    
                return child;
            }
        }
        // Move to the next sibling in the child list.
        child = child->subling_list;
    }
    return NULL;
}

// Probes for the topmost window covering the point (x,y).
// Traverses the top-level window list in reverse order using the "prev" pointer.
struct gws_window_d *wm_probe_window_at(int x, int y) {
    struct gws_window_d *w = last_window; // last_window is assumed to be the topmost
    
    while (w != NULL) {
        // Validate window.
        if (w->used == TRUE &&
            w->magic == 1234 &&
            w->state != WINDOW_STATE_MINIMIZED) {
                
           // Using the window frame rectangle (rcWindow) for hit-testing.
           if (x >= w->rcWindow.left && x < w->rcWindow.right &&
               y >= w->rcWindow.top  && y < w->rcWindow.bottom) {
                   
               // Before returning, probe any children of this window.
               struct gws_window_d *child = wm_probe_child_window(w, x, y);
               if (child != NULL)
                   return child;
                   
               return w; // No child intercepted the event; return the window.
           }
        }
        // Move backward in z-order.
        w = w->prev;
    }
    
    // If no window covers the point, return the root window.
    return __root_window;
}




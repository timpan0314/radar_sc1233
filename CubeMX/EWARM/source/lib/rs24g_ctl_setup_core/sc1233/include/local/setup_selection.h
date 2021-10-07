/*
 * Copyright (c) 2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * setup_selection.h
 */

#ifndef __SETUP_SELECTION_H__
#define __SETUP_SELECTION_H__
#define RS_ROM_LIMIT_64KB  1 //Tim for reduce ROM size
#if defined(RS_ROM_LIMIT_32KB)
#  include "setup_selection_32kb.h"
#elif defined(RS_ROM_LIMIT_64KB)
#  include "setup_selection_64kb.h"
#elif defined(RS_ROM_LIMIT_128KB)
#  include "setup_selection_128kb.h"
#elif defined(RS_ROM_LIMIT_192KB)
#  include "setup_selection_192kb.h"
#elif defined(RS_ROM_LIMIT_256KB)
#  include "setup_selection_256kb.h"
#else
#  include "setup_selection_all.h"
#endif

#endif	/* __SETUP_SELECTION_H__ */

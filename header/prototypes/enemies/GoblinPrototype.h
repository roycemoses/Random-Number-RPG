#ifndef __GOBLIN_PROTOTYPE__
#define __GOBLIN_PROTOTYPE__

#include "EnemyPrototype.h"

class GoblinPrototype : public EnemyPrototype 
{
    public:
        GoblinPrototype();
        virtual void Clone() { setName("Goblin"); setHealth(25); };
};

#endif // __GOBLIN_PROTOTYPE__
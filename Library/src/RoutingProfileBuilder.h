#ifndef ROUTINGPROFILEBUILDER_H_INCLUDED
#define ROUTINGPROFILEBUILDER_H_INCLUDED

#include <osmscout/Database.h>
#include <osmscout/routing/RoutingProfile.h>

#include "Router.h"

std::shared_ptr<osmscout::FastestPathRoutingProfile>
MakeRoutingProfile(const route_profile profile,
                   const osmscout::DatabaseRef& database);

#endif // ROUTINGPROFILEBUILDER_H_INCLUDED

/* put parameters in linked list and retrieve */

#include <ctime>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "proj.h"
#include "proj_internal.h"

/* create parameter list entry */
paralist *pj_mkparam(const char *str) {
    paralist *newitem;

    if ((newitem = (paralist *)malloc(sizeof(paralist) + strlen(str))) !=
        nullptr) {
        newitem->used = 0;
        newitem->next = nullptr;
        if (*str == '+')
            ++str;
        (void)strcpy(newitem->param, str);
    }
    return newitem;
}

/* As pj_mkparam, but payload ends at first whitespace, rather than at end of
 * <str> */
paralist *pj_mkparam_ws(const char *str, const char **next_str) {
    paralist *newitem;
    size_t len = 0;

    if (nullptr == str)
        return nullptr;

    /* Find start and length of string */
    while (isspace(*str))
        str++;
    if (*str == '+')
        str++;
    bool in_string = false;
    for (; str[len] != '\0'; len++) {
        if (in_string) {
            if (str[len] == '"' && str[len + 1] == '"') {
                len++;
            } else if (str[len] == '"') {
                in_string = false;
            }
        } else if (str[len] == '=' && str[len + 1] == '"') {
            in_string = true;
        } else if (isspace(str[len])) {
            break;
        }
    }

    if (next_str)
        *next_str = str + len;

    /* Use calloc to automagically 0-terminate the copy */
    newitem = (paralist *)calloc(1, sizeof(paralist) + len + 1);
    if (nullptr == newitem)
        return nullptr;
    memcpy(newitem->param, str, len);

    newitem->used = 0;
    newitem->next = nullptr;

    return newitem;
}

/**************************************************************************************/
paralist *pj_param_exists(paralist *list, const char *parameter) {
    /***************************************************************************************
        Determine whether a given parameter exists in a paralist. If it does,
    return a pointer to the corresponding list element - otherwise return 0.

        In support of the pipeline syntax, the search is terminated once a
    "+step" list element is reached, in which case a 0 is returned, unless the
    parameter searched for is actually "step", in which case a pointer to the
    "step" list element is returned.

        This function is equivalent to the pj_param (...) call with the "opt"
    argument set to the parameter name preceeeded by a 't'. But by using this
    one, one avoids writing the code allocating memory for a new copy of
    parameter name, and prepending the t (for compile time known names, this is
    obviously not an issue).
    ***************************************************************************************/
    paralist *next = list;
    const char *c = strchr(parameter, '=');
    size_t len = strlen(parameter);
    if (c)
        len = c - parameter;
    if (list == nullptr)
        return nullptr;

    for (next = list; next; next = next->next) {
        if (0 == strncmp(parameter, next->param, len) &&
            (next->param[len] == '=' || next->param[len] == 0)) {
            next->used = 1;
            return next;
        }
        if (0 == strcmp(parameter, "step"))
            return nullptr;
    }

    return nullptr;
}

/************************************************************************/
/*                              pj_param()                              */
/*                                                                      */
/*      Test for presence or get parameter value.  The first            */
/*      character in `opt' is a parameter type which can take the       */
/*      values:                                                         */
/*                                                                      */
/*       `t' - test for presence, return TRUE/FALSE in PROJVALUE.i      */
/*       `i' - integer value returned in PROJVALUE.i                    */
/*       `d' - simple valued real input returned in PROJVALUE.f         */
/*       `r' - degrees (DMS translation applied), returned as           */
/*             radians in PROJVALUE.f                                   */
/*       `s' - string returned in PROJVALUE.s                           */
/*       `b' - test for t/T/f/F, return in PROJVALUE.i                  */
/*                                                                      */
/*      Search is terminated when "step" is found, in which case        */
/*      0 is returned, unless "step" was the target searched for.       */
/*                                                                      */
/************************************************************************/

PROJVALUE pj_param(PJ_CONTEXT *ctx, paralist *pl, const char *opt) {

    int type;
    unsigned l;
    PROJVALUE value = {0};

    if (ctx == nullptr)
        ctx = pj_get_default_ctx();

    type = *opt++;

    if (nullptr == strchr("tbirds", type)) {
        fprintf(stderr, "invalid request to pj_param, fatal\n");
        exit(1);
    }

    pl = pj_param_exists(pl, opt);
    if (type == 't') {
        value.i = pl != nullptr;
        return value;
    }

    /* Not found */
    if (nullptr == pl) {
        /* Return value after the switch, so that the return path is */
        /* taken in all cases */
        switch (type) {
        case 'b':
        case 'i':
            value.i = 0;
            break;
        case 'd':
        case 'r':
            value.f = 0.;
            break;
        case 's':
            value.s = nullptr;
            break;
        }
        return value;
    }

    /* Found parameter - now find its value */
    pl->used |= 1;
    l = (int)strlen(opt);
    opt = pl->param + l;
    if (*opt == '=')
        ++opt;

    switch (type) {
    case 'i': /* integer input */
        value.i = atoi(opt);
        for (const char *ptr = opt; *ptr != '\0'; ++ptr) {
            if (!(*ptr >= '0' && *ptr <= '9')) {
                proj_context_errno_set(ctx,
                                       PROJ_ERR_INVALID_OP_ILLEGAL_ARG_VALUE);
                value.i = 0;
            }
        }
        break;
    case 'd': /* simple real input */
        value.f = pj_atof(opt);
        break;
    case 'r': /* degrees input */
        value.f = dmstor_ctx(ctx, opt, nullptr);
        break;
    case 's': /* char string */
        value.s = (char *)opt;
        break;
    case 'b': /* boolean */
        switch (*opt) {
        case 'F':
        case 'f':
            value.i = 0;
            break;
        case '\0':
        case 'T':
        case 't':
            value.i = 1;
            break;
        default:
            proj_context_errno_set(ctx, PROJ_ERR_INVALID_OP_ILLEGAL_ARG_VALUE);
            value.i = 0;
            break;
        }
        break;
    }
    return value;
}

/* Parse +t_final parameter with support for "now" keyword */
double pj_parse_t_final(PJ *P) {
    if (!pj_param(P->ctx, P->params, "tt_final").i) {
        return 0.0;
    }

    double t_final = pj_param(P->ctx, P->params, "dt_final").f;
    if (t_final != 0.0) {
        return t_final;
    }

    /* Check if "now" was specified instead of a numeric value */
    const char *t_final_str = pj_param(P->ctx, P->params, "st_final").s;
    if (!t_final_str || strcmp("now", t_final_str) != 0) {
        return 0.0;
    }

    /* Calculate t_final from current time */
    time_t now;
    struct tm date;
    time(&now);
#ifdef _WIN32
    localtime_s(&date, &now);
#else
    localtime_r(&now, &date);
#endif

    const int days_in_year =
        ((date.tm_year + 1900) % 4 == 0 &&
         ((date.tm_year + 1900) % 100 != 0 || (date.tm_year + 1900) % 400 == 0))
            ? 366
            : 365;

    return 1900.0 + date.tm_year +
           date.tm_yday / static_cast<double>(days_in_year);
}

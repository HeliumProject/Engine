#pragma once 

//
// http://www.eggheads.org/pipermail/eggdev/2000-August/017226.html
//
// Case insensitive wildcard match... the string with wildcards is the
// first arg. The string to test against is the second arg. This
// implentation was written by Jon Miles <jon@zetnet.net>. I've yet to see
// another wild-match implementation as simple as this... there's gotta be
// a bug or flaw in it somewhere?
//

inline bool WildcardMatch(const tchar *String1,const tchar *String2)
{
    bool    bStar = false;
    /* Set to true when processing a wildcard * in String1 */
    tchar   *StarPos;	
    /* Set this to the text just after the
    last star, so we can resurrect String1
    when we find that String2 isnt matching
    after a * earlier on.
    */

    /*	  Loop through each character in the string sorting out the
    wildcards. If a ? is found then just increment both strings. If
    a * is found then increment the second string until the first
    character matches, then continue as normal. This is where the
    algorithm gets a little more complicated. As matching
    *zetnet.co.uk to zdialup.zetnet.co.uk would incorrectly return
    false. To solve this i'm keeping a pointer to the string just
    after the last * so that when the two next characters from each
    string dont match, we reset String1 back to the string we
    should be looking for.
    */
    while(true)
    {
        switch(*String1)
        {
        case '*':
            {
                String1++;
                bStar = true;
                StarPos = (tchar *)String1;
                break;
            }

        case '?':
            {
                String1++;
                String2++;
                break;
            }

        case 0:	//	NULL terminator
            {
                if(!String2[0])
                {
                    /* End of both strings, so it matches. */
                    return true;
                }
                if(*(String1-1)=='*')
                {
                    /* The last character in String1 was a '*', so it matches. */
                    return true;
                }

                /* End of one string, but not the other, fails match. */
                return false;
                break;
            }

        default:
            {
                if(toupper(*String1)!=toupper(*String2))
                {
                    if(!String2[0])
                    {
                        /* End of String2 but not String1, doesnt match. */
                        return false;
                    }
                    if(bStar)
                    {
                        String2++;
                        if(!String2[0])
                            return false;

                        /* Reset String1 to just after the last '*'. */
                        String1 = StarPos;
                    }
                    else
                        return false;
                }
                else
                {
                    String1++;
                    String2++;
                }
                break;
            }
        }
    }

    //	shouldn't get here
    return false;
}

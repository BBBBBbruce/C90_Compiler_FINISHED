int cont(int a, int b)
{
    int c = 0;
    while(a < b)
    {
	a=a+1;
	if(a < b/2)
	{
	    continue;
	}
	else
	{
	    c =c+ 2;
	}
    }

    return c;
}

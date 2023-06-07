// PR c++/109172

// { dg-require-effective-target exceptions_enabled }

class Demo
{
  ~Demo();
};

int main()
{
  try
    {
      throw *new Demo;		// { dg-error private }
    }
  catch(const Demo& e) { }
}

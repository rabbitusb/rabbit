



int main(void);


void reset_handler(void)
{
    main();
}
void default_handler(void)
{

}
volatile const int int_vec[] __attribute__((section(".isr_vector"))) =
{
    0x20000200,
    (const int)reset_handler,
    (const int)default_handler
};

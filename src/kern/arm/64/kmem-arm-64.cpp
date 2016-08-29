IMPLEMENTATION [!noncont_mem]:

PUBLIC static
Address
Kmem::mmio_remap(Address phys)
{
  static Address ndev = 0;
  Address dm = Mem_layout::Registers_map_start + ndev;
  assert(dm < Mem_layout::Registers_map_end);

  ndev += Config::SUPERPAGE_SIZE;

  auto m = Kmem_space::kdir()->walk(Virt_addr(dm), Pte_ptr::Super_level);
  assert (!m.is_valid());
  assert (m.page_order() == Config::SUPERPAGE_SHIFT);
  Address phys_page = cxx::mask_lsb(phys, Config::SUPERPAGE_SHIFT);
  m.set_page(m.make_page(Phys_mem_addr(phys_page),
                         Page::Attr(Page::Rights::RWX(),
                                    Page::Type::Uncached(),
                                    Page::Kern::Global())));

  m.write_back_if(true, Mem_unit::Asid_kernel);

  return (phys & ~Config::SUPERPAGE_MASK) | (dm & Config::SUPERPAGE_MASK);
}
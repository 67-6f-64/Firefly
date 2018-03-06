#include "itemfilter_hooks.hpp"
#include "itemfilter_hook_data.hpp"

#include "itemfilter_data.hpp"
#include "detours.hpp"

namespace firefly
{
	bool Hook_CDropPool__OnDropEnterField(bool enable)
	{
		CDropPool__OnDropEnterField_t CDropPool__OnDropEnterField_hook = [](void* ecx, void* edx, CInPacket* iPacket) -> void
		{
			if (itemfilter_data::get_instance().enabled())
			{
				unsigned char nEnterType = iPacket->fetch_at<unsigned char>(OnDropEnterField_Offset_nEnterType);
				unsigned char bReal = ((nEnterType == 1) || (nEnterType == 2));

				unsigned char bIsMoney = iPacket->fetch_at<unsigned char>(OnDropEnterField_Offset_bIsMoney);
				unsigned int nInfo = iPacket->fetch_at<unsigned int>(OnDropEnterField_Offset_nInfo);

				if (bReal)
				{
					bool blacklist = (itemfilter_data::get_instance().itemfilter_type() == itemfilter_block_type_ids::blacklist);
					unsigned char scheme = itemfilter_data::get_instance().itemfilter_scheme();

					if (bIsMoney)
					{
						if (itemfilter_data::get_instance().enable_mesos_filter() && nInfo < itemfilter_data::get_instance().mesos_filter_value())
							return;
					}
					else
					{
						if (scheme == itemfilter_type_ids::filter_everything)
						{
							if (blacklist)
								return;
						}
						else
						{
							if (scheme == itemfilter_type_ids::filter_by_type || scheme == itemfilter_type_ids::custom_filter)
							{
								if ((scheme == itemfilter_type_ids::custom_filter && itemfilter_data::get_instance().drop_list().exists(nInfo)) ||
									(nInfo >= 1000000 && nInfo < 2000000 && itemfilter_data::get_instance().block_type_equip()) ||
									(nInfo >= 2000000 && nInfo < 3000000 && itemfilter_data::get_instance().block_type_use()) ||
									(nInfo >= 4000000 && nInfo < 5000000 && itemfilter_data::get_instance().block_type_etc()))
								{
									if (blacklist)
										return;
									else
										return CDropPool__OnDropEnterField(ecx, edx, iPacket);
								}
							}

							if (!blacklist)
								return;
						}
					}
				}
				else
				{
					/* Filter (bReal == false) results? */
					/* Returning = Bubble-items doesn't appear */
					return;
				}
			}

			return CDropPool__OnDropEnterField(ecx, edx, iPacket);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CDropPool__OnDropEnterField), CDropPool__OnDropEnterField_hook);
	}

	void set_itemfilter_hooks(bool enable)
	{
		Hook_CDropPool__OnDropEnterField(enable);
	}
}
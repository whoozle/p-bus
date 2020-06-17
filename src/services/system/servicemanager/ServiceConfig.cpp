#include <system/servicemanager/ServiceConfig.h>
#include <toolkit/core/ByteArray.h>
#include <toolkit/io/File.h>
#include <toolkit/log/Logger.h>
#include <toolkit/text/String.h>

namespace pbus { namespace system { namespace servicemanager
{

	Config::Config(const std::string &path)
	{
		io::File f(path);
		auto size = f.GetStatus().st_size;
		ByteArray data(size);
		f.Read(data);
		std::string text(data.begin(), data.end());
		log::Logger log("line");
		text::Split<std::string>(text, "\n", [&](std::string && line) {
			if (line.empty())
				return;
			Dependencies.insert(line);
		});
	}

}}}

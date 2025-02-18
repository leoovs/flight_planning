#include <catch2/catch_all.hpp>
#include <uavpf/debug/log_record.h>
#include <uavpf/debug/logger.h>
#include <uavpf/format_str.h>

#include <vector>

TEST_CASE("Mock logging")
{
	using uavpf::FormatStr;
	using uavpf::FormattedLog;
	using uavpf::LogEmitter;
	using uavpf::LogFormatter;
	using uavpf::LogLevel;
	using uavpf::LogRecord;
	using uavpf::LogSource;
	using uavpf::Logger;
	using uavpf::MakeLogRecord;

	using VectorOfLogs = std::vector<FormattedLog>;

	class MockLogFormatter : public LogFormatter
	{
	public:
		~MockLogFormatter() override = default;

		FormattedLog Format(LogSource source, const LogRecord& record) override
		{
			FormattedLog log;
			log.Level = record.Level;
			log.Source = source;
			log.Log = FormatStr("MockMe: %s", record.Message.data());

			return log;
		}
	};

	class MockLogEmitter : public LogEmitter
	{
	public:
		using VectorBackInsertIter = std::back_insert_iterator<VectorOfLogs>;

		MockLogEmitter(VectorBackInsertIter backInserter)
			: mBackInserter(backInserter)
		{
		}

		void Emit(const FormattedLog& log) override
		{
			mBackInserter = log;
		}

	private:
		VectorBackInsertIter mBackInserter;
	};

	SECTION("Formatter & emitter work smoothly")
	{
		VectorOfLogs logStorage;

		auto logsAreEqual = [](const FormattedLog& x, const FormattedLog& y) -> bool
			{
				return x.Level == y.Level
					&& x.Source == y.Source
					&& x.Log == y.Log;
			};

		auto formatter = std::make_shared<MockLogFormatter>();
		auto emitter = std::make_shared<MockLogEmitter>(std::back_inserter(logStorage));

		Logger mockLogger(std::move(formatter), std::move(emitter), LogSource::Mock);

		mockLogger.Log(MakeLogRecord("<Mock1.cpp>", 150, LogLevel::Warning, "%s", "MockMessage1"));
		mockLogger.Log(MakeLogRecord("<Mock2.cpp>", 300, LogLevel::Info, "%s", "MockMessage2"));

		FormattedLog expectedLog1;
		expectedLog1.Level = LogLevel::Warning;
		expectedLog1.Source = LogSource::Mock;
		expectedLog1.Log = "MockMe: MockMessage1";

		FormattedLog expectedLog2;
		expectedLog2.Level = LogLevel::Info;
		expectedLog2.Source = LogSource::Mock;
		expectedLog2.Log = "MockMe: MockMessage2";

		REQUIRE(logsAreEqual(expectedLog1, logStorage.at(0)));
		REQUIRE(logsAreEqual(expectedLog2, logStorage.at(1)));
	}
}


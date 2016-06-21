<?php

namespace Ecoli;

class Log implements SingletonContract
{
	/**
	 * Core application object.
	 *
	 * @var Ecoli\Application
	 */
	static $app;

	/**
	 * @var Singleton The reference to *Singleton* instance of this class
	 */
	private static $instance;

	/**
	 * Returns the *Singleton* instance of this class.
	 *
	 * @return Singleton The *Singleton* instance.
	 */
	public static function getInstance()
	{
		if (null === static::$instance) {
			static::$instance = new static();
		}
		
		return static::$instance;
	}

	/**
	 * Log message.
	 *
	 * @param  string  $message
	 * @return void
	 */
	public static function info($message)
	{

	}

	/**
	 * Register any hook and run initializer routines.
	 *
	 * @return void
	 */
	public function boot()
	{
		
	}

	/**
	 * Inject core application.
	 *
	 * @return void
	 */
	public function inject($core)
	{
		$this->app = $core;
	}

	/**
	 * Protected constructor to prevent creating a new instance of the
	 * *Singleton* via the `new` operator from outside of this class.
	 */
	protected function __construct()
	{
		
	}

	/**
	 * Private clone method to prevent cloning of the instance of the
	 * *Singleton* instance.
	 *
	 * @return void
	 */
	private function __clone()
	{

	}

	/**
	 * Private unserialize method to prevent unserializing of the *Singleton*
	 * instance.
	 *
	 * @return void
	 */
	private function __wakeup()
	{

	}

}